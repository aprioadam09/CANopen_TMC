#include "stm32f4xx.h"
#include "rcc.h"
#include "can.h"
#include <string.h> // For memcmp

// Global volatile variables for DEBUGGING ONLY
volatile can_msg_t sent_msg_for_debug;
volatile can_msg_t received_msg_for_debug;
volatile int test_status = 0; // 0=Pending, 1=Success, -1=Fail

void delay(volatile uint32_t count) {
    while(count--);
}

int main(void) {
    // 1. Basic hardware initialization
    rcc_system_clock_config();

    // 2. Initialize the CAN peripheral in LOOPBACK mode
    can_init(true);

    // 3. Prepare the test message using a LOCAL, NON-VOLATILE variable
    can_msg_t tx_msg;
    tx_msg.id = 0x123;
    tx_msg.len = 8;
    tx_msg.data[0] = 0xDE;
    tx_msg.data[1] = 0xAD;
    tx_msg.data[2] = 0xBE;
    tx_msg.data[3] = 0xEF;
    tx_msg.data[4] = 0xFE;
    tx_msg.data[5] = 0xED;
    tx_msg.data[6] = 0xFA;
    tx_msg.data[7] = 0xCE;

    // Copy to the global variable for debugging, if needed
    memcpy((void*)&sent_msg_for_debug, &tx_msg, sizeof(can_msg_t));

    // 4. Send the message
    can_send(&tx_msg);

    // 5. Wait a short moment for the interrupt to process the message
    delay(1000000);

    // 6. Attempt to receive the message into a LOCAL, NON-VOLATILE variable
    can_msg_t rx_msg;
    memset(&rx_msg, 0, sizeof(can_msg_t)); // Clear it first
    size_t received_count = can_recv(&rx_msg, 1);

    // Copy to the global variable for debugging
    if (received_count > 0) {
        memcpy((void*)&received_msg_for_debug, &rx_msg, sizeof(can_msg_t));
    }

    // 7. Verify the result
    if (received_count > 0) {
        // We received a message. Now check if it's identical.
        if ( (tx_msg.id == rx_msg.id) &&
             (tx_msg.len == rx_msg.len) &&
             (memcmp(tx_msg.data, rx_msg.data, tx_msg.len) == 0) )
        {
            test_status = 1; // Success!
        } else {
            test_status = -1; // Fail: Data mismatch
        }
    } else {
        test_status = -1; // Fail: Did not receive any message
    }

    // 8. Stop here for debugging
    while(1) {
        // Halt processor for inspection
    }
    return 0;
}
