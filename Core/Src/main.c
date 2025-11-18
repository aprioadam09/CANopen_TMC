#include "stm32f4xx.h"
#include "rcc.h"
#include "can.h"
#include <string.h>

// Global volatile variables for debugging the reception part
volatile can_msg_t received_msg_for_debug;
volatile uint32_t received_message_count = 0;

void delay(volatile uint32_t count) {
    while(count--);
}

int main(void) {
    // 1. Basic hardware initialization
    rcc_system_clock_config();

    // 2. Initialize the CAN peripheral in NORMAL mode
    can_init(false);

    // 3. Prepare message structures
    can_msg_t tx_msg; // For sending
    can_msg_t rx_msg; // For receiving

    uint8_t counter = 0;

    while(1) {
        // --- PART A: TRANSMISSION ---
        // Prepare a message to transmit from the STM32
        tx_msg.id = 0x123; // Our device's ID
        tx_msg.len = 1;
        tx_msg.data[0] = counter; // Data payload will increment each second

        // Send the message
        can_send(&tx_msg);

        // --- PART B: RECEPTION ---
        // Check if any message has been received from the outside world
        if (can_recv(&rx_msg, 1) > 0) {
            // A message was received! Copy it to our debug variable.
            memcpy((void*)&received_msg_for_debug, &rx_msg, sizeof(can_msg_t));
            received_message_count++;
            // A breakpoint can be placed here to inspect the received message
        }

        // Increment the counter for the next transmission
        counter++;

        // Wait for approximately 1 second
        delay(8000000);
    }
    return 0;
}
