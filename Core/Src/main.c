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
#include <lely/co/rpdo.h>
#include <lely/co/tpdo.h>
#include <lely/co/time.h>
#include <lely/co/val.h>

// --- C Standard Library Includes ---
#include <time.h>

// NMT State constants
#define CO_NMT_ST_BOOTUP         0x00
#define CO_NMT_ST_STOP           0x04  // PRE-OPERATIONAL
#define CO_NMT_ST_START          0x05  // OPERATIONAL

// [STATE MACHINE] Definisi state CiA 402, sesuai diagram
typedef enum {
    PDS_STATE_NOT_READY_TO_SWITCH_ON,
    PDS_STATE_SWITCH_ON_DISABLED,
    PDS_STATE_READY_TO_SWITCH_ON,
    PDS_STATE_SWITCHED_ON,
    PDS_STATE_OPERATION_ENABLED,
    PDS_STATE_QUICK_STOP_ACTIVE,
    PDS_STATE_FAULT_REACTION_ACTIVE,
    PDS_STATE_FAULT
} pds_state_t;

// [STATE MACHINE] Bit-bit penting di Statusword (Objek 0x6041)
#define SW_READY_TO_SWITCH_ON   (1 << 0)
#define SW_SWITCHED_ON          (1 << 1)
#define SW_OPERATION_ENABLED    (1 << 2)
#define SW_FAULT                (1 << 3)
#define SW_VOLTAGE_ENABLED      (1 << 4)
#define SW_QUICK_STOP           (1 << 5)
#define SW_SWITCH_ON_DISABLED   (1 << 6)
#define SW_TARGET_REACHED       (1 << 10)

// [STATE MACHINE] Perintah dari Controlword (Objek 0x6040)
#define CW_CMD_SHUTDOWN         0x0006
#define CW_CMD_SWITCH_ON        0x0007
#define CW_CMD_DISABLE_VOLTAGE  0x0000
#define CW_CMD_QUICK_STOP       0x0002
#define CW_CMD_DISABLE_OP       0x0007
#define CW_CMD_ENABLE_OP        0x000F
#define CW_CMD_FAULT_RESET      0x0080

// [STATE MACHINE] Bit-bit penting di RAMP_STAT TMC5160
#define RAMP_STAT_POSITION_REACHED (1 << 9)

// [STATE MACHINE] Variabel global untuk state machine
static volatile pds_state_t current_state = PDS_STATE_NOT_READY_TO_SWITCH_ON;
static volatile uint16_t statusword = 0;
static int8_t current_mode_op = 0;
static bool is_homing_attained = false; // Menyimpan status apakah homing sudah sukses
static uint16_t previous_controlword = 0;

// Global pointers for the Lely CANopen stack components
static can_net_t *net = NULL;
static co_dev_t *dev = NULL;
static co_nmt_t *nmt = NULL;

static int on_can_send(const struct can_msg *msg, void *data);
static void on_nmt_cs(co_nmt_t *nmt, co_unsigned8_t cs, void *data);
static void on_time(co_time_t *time, const struct timespec *tp, void *data);
static co_unsigned32_t on_read_actual_pos(const co_sub_t *sub, struct co_sdo_req *req, void *data);
static co_unsigned32_t on_write_target_pos(co_sub_t *sub, struct co_sdo_req *req, void *data);
static co_unsigned32_t on_read_statusword(const co_sub_t *sub, struct co_sdo_req *req, void *data);
static co_unsigned32_t on_write_controlword(co_sub_t *sub, struct co_sdo_req *req, void *data);
static co_unsigned32_t on_write_mode_op(co_sub_t *sub, struct co_sdo_req *req, void *data);
static void update_statusword(void);

// Core logic functions (shared between SDO and PDO)
static bool process_controlword(uint16_t command);
static void process_mode_of_operation(int8_t mode);
static bool process_target_position(int32_t target_pos);
static void execute_target_position(void);

// PDO callback functions
static void on_rpdo1_write(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data);
static void on_rpdo2_write(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data);
static void on_rpdo3_write(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data);
static void register_rpdo_callbacks(void);

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

    tmc5160_write_register(TMC5160_XACTUAL, 0);

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

    co_sub_set_up_ind(co_dev_find_sub(dev, 0x6041, 0x00), &on_read_statusword, NULL);

    co_sub_set_dn_ind(co_dev_find_sub(dev, 0x6040, 0x00), &on_write_controlword, NULL);

    co_sub_set_dn_ind(co_dev_find_sub(dev, 0x6060, 0x00), &on_write_mode_op, NULL);

    register_rpdo_callbacks();

    current_state = PDS_STATE_SWITCH_ON_DISABLED;
    uint32_t last_tpdo_time = 0;

    // --- Main Application Loop (Lely Scheduler) ---
    while(1) {
    	// 3. Get the current time and process any time-based events in the Lely stack
		struct timespec now;
		get_time(&now);
		can_net_set_time(net, &now);

        struct can_msg rx_msg;

        // 1. Check our CAN driver's ring buffer for any new messages
        if (can_recv(&rx_msg, 1)) {
            // 2. If a message exists, pass it to the Lely stack for processing
            can_net_recv(net, &rx_msg);
        }

        // 4. Update statusword (tanpa trigger TPDO)
        update_statusword();

        // 5. ← TAMBAHAN BARU: Trigger TPDO secara periodic (setiap 100ms)
        uint32_t current_time = millis();
        if (current_time - last_tpdo_time >= 100) {
            last_tpdo_time = current_time;

            // ✨ Check NMT state before sending TPDO
            co_unsigned8_t nmt_state = co_nmt_get_st(nmt);

            if (nmt_state == CO_NMT_ST_START) {  // Only in OPERATIONAL
                co_tpdo_t *tpdo2 = co_nmt_get_tpdo(nmt, 2);
                if (tpdo2) {
                    int32_t actual_pos = tmc5160_read_register(TMC5160_XACTUAL);
                    co_dev_set_val_i32(dev, 0x6064, 0x00, actual_pos);
                    co_tpdo_event(tpdo2);
                }
            }
        }
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
 * @brief Callback executed on SDO write to Target Position (0x607A)
 */
static co_unsigned32_t on_write_target_pos(co_sub_t *sub, struct co_sdo_req *req, void *data) {
    (void)data;
    co_unsigned32_t ac = 0;

    int32_t target_pos;
    if (co_sdo_req_dn_val(req, CO_DEFTYPE_INTEGER32, &target_pos, &ac) == -1) {
        return ac;
    }

    // HANYA SIMPAN, tidak eksekusi
    if (!process_target_position(target_pos)) {
        return CO_SDO_AC_DATA_DEV;
    }

    return 0;
}

/**
 * @brief [STATE MACHINE] Updates the global 'statusword' variable based on the current state.
 *        This function also includes logic for other dynamic bits like 'Target Reached'.
 */
/**
 * @brief [STATE MACHINE] Updates the global 'statusword' variable based on the current state.
 */
static void update_statusword(void) {
    uint16_t base_sw = 0;

    // 1. Tentukan status dasar berdasarkan State Machine (PDS State)
    switch (current_state) {
        case PDS_STATE_NOT_READY_TO_SWITCH_ON:
            base_sw = 0;
            break;
        case PDS_STATE_SWITCH_ON_DISABLED:
            base_sw = SW_SWITCH_ON_DISABLED;
            break;
        case PDS_STATE_READY_TO_SWITCH_ON:
            base_sw = SW_READY_TO_SWITCH_ON | SW_QUICK_STOP;
            break;
        case PDS_STATE_SWITCHED_ON:
            base_sw = SW_READY_TO_SWITCH_ON | SW_SWITCHED_ON | SW_QUICK_STOP;
            break;
        case PDS_STATE_OPERATION_ENABLED:
            base_sw = SW_READY_TO_SWITCH_ON | SW_SWITCHED_ON | SW_OPERATION_ENABLED | SW_QUICK_STOP | SW_VOLTAGE_ENABLED;
            break;
        case PDS_STATE_QUICK_STOP_ACTIVE:
            base_sw = SW_READY_TO_SWITCH_ON | SW_SWITCHED_ON | SW_OPERATION_ENABLED;
            break;
        case PDS_STATE_FAULT_REACTION_ACTIVE:
            base_sw = SW_READY_TO_SWITCH_ON | SW_SWITCHED_ON | SW_OPERATION_ENABLED | SW_FAULT;
            break;
        case PDS_STATE_FAULT:
            base_sw = SW_FAULT;
            break;
    }

    // 2. Logika Tambahan (Hanya jika drive aktif/Enabled)
    if (current_state == PDS_STATE_OPERATION_ENABLED) {
        // A. Cek Status Fisik Hardware (Apakah motor berhenti?)
        int32_t ramp_stat = tmc5160_read_register(TMC5160_RAMP_STAT);
        if (ramp_stat & RAMP_STAT_POSITION_REACHED) {
            base_sw |= SW_TARGET_REACHED;
        }

        // B. Cek Logika Khusus Mode Homing
        if (current_mode_op == 6) {
            if (is_homing_attained) {
                base_sw |= (1 << 12);
                base_sw |= SW_TARGET_REACHED;
            }
        }
    }

    // 3. Update statusword
    statusword = base_sw;

    // 4. Update OD (tapi TIDAK trigger TPDO di sini!)
    // TPDO akan di-trigger secara manual di tempat yang tepat
    co_dev_set_val_u16(dev, 0x6041, 0x00, statusword);
}

/**
 * @brief [GLUE LOGIC] Callback executed on SDO read for Statusword (0x6041).
 */
static co_unsigned32_t on_read_statusword(const co_sub_t *sub, struct co_sdo_req *req, void *data) {
    (void)sub;
    (void)data;

    co_unsigned32_t ac = 0;

    uint16_t sw_copy = statusword;

    // Sediakan nilai dari variabel global 'statusword' saat ini.
    co_sdo_req_up_val(req, CO_DEFTYPE_UNSIGNED16, &sw_copy, &ac);

    return ac;
}

/**
 * @brief [STATE MACHINE] Callback executed on SDO write to Controlword (0x6040).
 *        This is the core of the CiA 402 state machine logic.
 */
static co_unsigned32_t on_write_controlword(co_sub_t *sub, struct co_sdo_req *req, void *data) {
    (void)data;
    co_unsigned32_t ac = 0;

    uint16_t command;
    if (co_sdo_req_dn_val(req, CO_DEFTYPE_UNSIGNED16, &command, &ac) == -1) {
        return ac;
    }

    co_sub_dn(sub, &command);

    if (!process_controlword(command)) {
        return CO_SDO_AC_DATA_DEV;
    }

    return 0;
}

// Callback saat Master menulis ke 0x6060 (Modes of Operation)
// Hapus 'const' pada parameter pertama (co_sub_t *sub)
static co_unsigned32_t on_write_mode_op(co_sub_t *sub, struct co_sdo_req *req, void *data) {
    (void)data;
    co_unsigned32_t ac = 0;
    int8_t mode;

    if (co_sdo_req_dn_val(req, CO_DEFTYPE_INTEGER8, &mode, &ac) == -1) {
        return ac;
    }

    co_sub_dn(sub, &mode);
    process_mode_of_operation(mode);

    return 0;
}

/**
 * @brief Core logic untuk memproses Controlword
 * @param command Nilai Controlword yang baru
 * @return true jika berhasil, false jika ditolak
 */
static bool process_controlword(uint16_t command) {
    // --- HOMING LOGIC ---
    if (current_mode_op == 6 && (command & 0x0010)) {
        tmc5160_write_register(TMC5160_XACTUAL, 0);
        tmc5160_write_register(TMC5160_XTARGET, 0);
        is_homing_attained = true;
        statusword |= (1 << 12) | (1 << 10);
        co_dev_set_val_u16(dev, 0x6041, 0x00, statusword);

        previous_controlword = command;
        return true;
    }

    if (current_mode_op != 6) {
        statusword &= ~(1 << 12);
        co_dev_set_val_u16(dev, 0x6041, 0x00, statusword);
    }

    // --- PROFILE POSITION MODE - DETEKSI RISING EDGE BIT 4 ---
	if (current_mode_op == 1) {  // Profile Position Mode
		bool bit4_previous = (previous_controlword & 0x0010) != 0;
		bool bit4_current = (command & 0x0010) != 0;

		// Rising edge terdeteksi: 0 → 1
		if (!bit4_previous && bit4_current) {
			execute_target_position();  // SEKARANG baru eksekusi!
		}
	}

    // --- STATE MACHINE TRANSITIONS ---
    switch (current_state) {
        case PDS_STATE_SWITCH_ON_DISABLED:
            if (command == CW_CMD_SHUTDOWN) {
                current_state = PDS_STATE_READY_TO_SWITCH_ON;
            }
            break;

        case PDS_STATE_READY_TO_SWITCH_ON:
            if (command == CW_CMD_SWITCH_ON) {
                current_state = PDS_STATE_SWITCHED_ON;
            }
            break;

        case PDS_STATE_SWITCHED_ON:
            if (command == CW_CMD_ENABLE_OP) {
                current_state = PDS_STATE_OPERATION_ENABLED;
                tmc5160_set_driver_enabled(true);
            } else if (command == CW_CMD_SHUTDOWN) {
                current_state = PDS_STATE_READY_TO_SWITCH_ON;
            }
            break;

        case PDS_STATE_OPERATION_ENABLED:
            if (command == CW_CMD_DISABLE_OP) {
                current_state = PDS_STATE_SWITCHED_ON;
                tmc5160_set_driver_enabled(false);
            } else if (command == CW_CMD_SHUTDOWN) {
                current_state = PDS_STATE_READY_TO_SWITCH_ON;
                tmc5160_set_driver_enabled(false);
            }
            break;

        default:
            break;
    }

    update_statusword();

    previous_controlword = command;

    // ← TAMBAHAN BARU: Trigger TPDO setelah state transition
	co_tpdo_t *tpdo1 = co_nmt_get_tpdo(nmt, 1);
	if (tpdo1) {
		co_tpdo_event(tpdo1);  // Kirim statusword update segera
	}
    return true;
}

/**
 * @brief Core logic untuk memproses Mode of Operation
 * @param mode Mode baru (1=PP, 6=Homing, dll)
 */
static void process_mode_of_operation(int8_t mode) {
    current_mode_op = mode;

    co_sub_t *sub_disp = (co_sub_t *)co_dev_find_sub(dev, 0x6061, 0x00);
    if (sub_disp) {
        co_sub_set_val_i8(sub_disp, mode);
    }
}

/**
 * @brief Menyimpan target position baru ke OD, TANPA menggerakkan motor
 * @param target_pos Posisi target dalam pulses
 * @return true jika diterima, false jika ditolak
 */
static bool process_target_position(int32_t target_pos) {
    if (current_state != PDS_STATE_OPERATION_ENABLED) {
        return false;
    }

    // Hanya simpan ke Object Dictionary, BELUM gerakkan motor
    co_dev_set_val_i32(dev, 0x607A, 0x00, target_pos);

    // Clear bit Target Reached karena ada setpoint baru (belum dieksekusi)
    statusword &= ~SW_TARGET_REACHED;
    co_dev_set_val_u16(dev, 0x6041, 0x00, statusword);

    return true;
}

/**
 * @brief EKSEKUSI gerakan ke target position yang sudah disimpan
 * @note Hanya dipanggil saat rising edge bit 4 terdeteksi
 */
static void execute_target_position(void) {
    // Baca target position dari OD
    int32_t target_pos = co_dev_get_val_i32(dev, 0x607A, 0x00);

    // EKSEKUSI gerakan fisik
    tmc5160_write_register(TMC5160_XTARGET, target_pos);

    // Clear bit Target Reached karena gerakan baru dimulai
    statusword &= ~SW_TARGET_REACHED;
    co_dev_set_val_u16(dev, 0x6041, 0x00, statusword);

    // Trigger TPDO untuk broadcast perubahan status
    co_tpdo_t *tpdo1 = co_nmt_get_tpdo(nmt, 1);
    if (tpdo1) {
        co_tpdo_event(tpdo1);
    }
}

/**
 * @brief Callback untuk RPDO1 - Controlword only
 */
static void on_rpdo1_write(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data) {
    (void)pdo;
    (void)ptr;
    (void)n;
    (void)data;

    if (ac != 0) return;

    uint16_t command = co_dev_get_val_u16(dev, 0x6040, 0x00);
    process_controlword(command);
}

/**
 * @brief Callback untuk RPDO2 - Controlword + Mode of Operation
 */
static void on_rpdo2_write(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data) {
    (void)pdo;
    (void)ptr;
    (void)n;
    (void)data;

    if (ac != 0) return;

    int8_t mode = co_dev_get_val_i8(dev, 0x6060, 0x00);
    uint16_t command = co_dev_get_val_u16(dev, 0x6040, 0x00);

    process_mode_of_operation(mode);
    process_controlword(command);
}

/**
 * @brief Callback untuk RPDO3 - Controlword + Target Position
 */
static void on_rpdo3_write(co_rpdo_t *pdo, co_unsigned32_t ac, const void *ptr, size_t n, void *data) {
    (void)pdo;
    (void)ptr;
    (void)n;
    (void)data;

    if (ac != 0) return;

	// 1. Simpan target position dulu (BELUM eksekusi)
	int32_t target_pos = co_dev_get_val_i32(dev, 0x607A, 0x00);
	process_target_position(target_pos);

	// 2. Process controlword (akan deteksi rising edge bit 4 dan eksekusi jika ada)
	uint16_t command = co_dev_get_val_u16(dev, 0x6040, 0x00);
	process_controlword(command);
}

/**
 * @brief Register semua RPDO callbacks
 */
static void register_rpdo_callbacks(void) {
    co_rpdo_t *rpdo1 = co_nmt_get_rpdo(nmt, 1);
    co_rpdo_t *rpdo2 = co_nmt_get_rpdo(nmt, 2);
    co_rpdo_t *rpdo3 = co_nmt_get_rpdo(nmt, 3);

    if (rpdo1) {
        co_rpdo_set_ind(rpdo1, &on_rpdo1_write, NULL);
    }
    if (rpdo2) {
        co_rpdo_set_ind(rpdo2, &on_rpdo2_write, NULL);
    }
    if (rpdo3) {
        co_rpdo_set_ind(rpdo3, &on_rpdo3_write, NULL);
    }
}
