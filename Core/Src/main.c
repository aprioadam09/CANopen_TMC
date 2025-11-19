#include "rcc.h"
#include "can.h"
#include "sdev.h"
#include "systick.h"
#include "spi.h"
#include "tmc5160.h"

// --- Lely CANopen Includes ---
#include <lely/co/dev.h>
#include <lely/co/nmt.h>
#include <lely/co/sdo.h>
#include <lely/co/time.h>

// --- C Standard Library Includes ---
#include <time.h>

// Global pointers for the Lely CANopen stack components
static can_net_t *net = NULL;
static co_dev_t *dev = NULL;
static co_nmt_t *nmt = NULL;

static int on_can_send(const struct can_msg *msg, void *data);
static void on_nmt_cs(co_nmt_t *nmt, co_unsigned8_t cs, void *data);
static void on_time(co_time_t *time, const struct timespec *tp, void *data);
static co_unsigned32_t on_read_actual_pos(const co_sub_t *sub, struct co_sdo_req *req, void *data);
static co_unsigned32_t on_write_target_pos(co_sub_t *sub, struct co_sdo_req *req, void *data);

/**
 * @brief Retrieves the current system time in milliseconds and converts it
 *        to the 'struct timespec' format required by Lely.
 * @param tp Pointer to the timespec structure to be filled.
 */
static void get_time(struct timespec *tp) {
    uint32_t ms = millis();
    tp->tv_sec = ms / 1000;
    tp->tv_nsec = (ms % 1000) * 1000000;
}

int main(void) {
    // --- Hardware Initialization (non-HAL) ---
    rcc_system_clock_config();
    systick_init();

    spi1_init();
    can_init(false); // Initialize CAN in normal bus mode

    tmc5160_init();

    // --- Motion Profile Configuration ---
	tmc5160_write_register(TMC5160_V1, 0);
	tmc5160_write_register(TMC5160_AMAX, 1000);
	tmc5160_write_register(TMC5160_DMAX, 1000);
	tmc5160_write_register(TMC5160_D1, 1000);
	tmc5160_write_register(TMC5160_VMAX, 51200);
	tmc5160_write_register(TMC5160_VSTOP, 100);

	// Add a zero-wait time for smooth direction reversals
	tmc5160_write_register(TMC5160_TZEROWAIT, 5000);

	// Set RAMPMODE to Positioning Mode
	tmc5160_write_register(TMC5160_RAMPMODE, 0);

    // --- Lely CANopen Stack Initialization ---

    // 1. Create the network interface
    net = can_net_create();

    // 2. Set the function that Lely will call to send a CAN frame
    can_net_set_send_func(net, &on_can_send, NULL);

    // 3. Create a CANopen device from our static Object Dictionary
    dev = co_dev_create_from_sdev(&slave_sdev);

    // 4. Create and start the NMT (Network Management) service
    nmt = co_nmt_create(net, dev);

    // Start the NMT service by resetting the node (triggers boot-up message).
    co_nmt_cs_ind(nmt, CO_NMT_CS_RESET_NODE);

    // Set the NMT indication function to handle commands from the master.
    co_nmt_set_cs_ind(nmt, &on_nmt_cs, NULL);

    // Set the TIME indication function.
    co_time_set_ind(co_nmt_get_time(nmt), &on_time, NULL);

    co_sub_set_up_ind(co_dev_find_sub(dev, 0x6064, 0x00), &on_read_actual_pos, NULL);

    co_sub_set_dn_ind(co_dev_find_sub(dev, 0x607A, 0x00), &on_write_target_pos, NULL);

    // --- Main Application Loop (Lely Scheduler) ---
    while(1) {
        struct can_msg rx_msg;

        // 1. Check our CAN driver's ring buffer for any new messages
        if (can_recv(&rx_msg, 1)) {
            // 2. If a message exists, pass it to the Lely stack for processing
            can_net_recv(net, &rx_msg);
        }

        // 3. Get the current time and process any time-based events in the Lely stack
        //    (e.g., sending the scheduled Heartbeat message)
        struct timespec now;
        get_time(&now);
        can_net_set_time(net, &now);
    }

    return 0;
}

/**
 * @brief Wrapper function to bridge our can_send() to Lely's can_send_func_t.
 * @param msg  Pointer to the CAN message provided by Lely.
 * @param data User-defined data pointer (unused).
 * @return 0 on success, -1 on failure.
 */
static int on_can_send(const struct can_msg *msg, void *data) {
    (void)data;
    if (can_send(msg) == 1) {
        return 0; // Success
    }
    return -1; // Failure
}

/**
 * @brief Callback function executed by Lely when an NMT command is received.
 * @param cs The command specifier (e.g., CO_NMT_CS_RESET_NODE).
 */
static void on_nmt_cs(co_nmt_t *nmt, co_unsigned8_t cs, void *data) {
    (void)nmt;
    (void)data;

    if (cs == CO_NMT_CS_RESET_NODE || cs == CO_NMT_CS_RESET_COMM) {
        NVIC_SystemReset();
    }
}

/**
 * @brief TIME stamp service indication function.
 * @note  This is a placeholder, as the Discovery board does not have a
 *        battery-backed Real-Time Clock to be set.
 */
static void on_time(co_time_t *time, const struct timespec *tp, void *data)
{
	(void)time;
	(void)tp;
	(void)data;
}

/**
 * @brief Callback function executed by Lely on an SDO read request for object 0x6064.
 *        This function reads the actual motor position from the TMC5160 and
 *        provides it to the Lely stack to be sent back to the master.
 */
static co_unsigned32_t on_read_actual_pos(const co_sub_t *sub, struct co_sdo_req *req, void *data) {
    (void)sub; // Unused
    (void)data; // Unused

    co_unsigned32_t ac = 0; // Abort Code, 0 = success

    int32_t actual_pos = tmc5160_read_register(TMC5160_XACTUAL);

    co_sdo_req_up_val(req, CO_DEFTYPE_INTEGER32, &actual_pos, &ac);

    return ac;
}

/**
 * @brief Callback function executed by Lely on an SDO write request for object 0x607A.
 *        This function receives the target position from the master, writes it to
 *        the local Object Dictionary, and then commands the TMC5160 to move.
 */
static co_unsigned32_t on_write_target_pos(co_sub_t *sub, struct co_sdo_req *req, void *data) {
    (void)data; // Unused

    co_unsigned32_t ac = 0; // Abort Code, 0 = success

    int32_t target_pos;
    if (co_sdo_req_dn_val(req, CO_DEFTYPE_INTEGER32, &target_pos, &ac) == -1) {
        return ac; // Return an error code if extraction fails
    }

    co_sub_dn(sub, &target_pos);

    tmc5160_write_register(TMC5160_XTARGET, target_pos);

    return ac;
}
