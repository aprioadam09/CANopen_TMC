#include "stm32f4xx.h"
#include "rcc.h"
#include "spi.h"

// Global volatile variables for easy debugging
volatile uint8_t spi_status_byte = 0;
volatile uint32_t ioin_register_value = 0;

// Temporary array to hold the raw received bytes
volatile uint8_t received_datagram[5] = {0};

void delay(volatile uint32_t count) {
    while(count--);
}

int main(void) {
    // 1. Configure system clock
    rcc_system_clock_config();

    // 2. Initialize SPI1
    spi1_init();

    // Give the TMC5160 a moment to power up fully
    delay(1000000);

    // --- TRANSACTION 1: Request to read the IOIN register (addr 0x04) ---
    // The data received during this transaction is not useful yet.
    spi1_cs_select();
    spi1_transfer(0x04); // Address 0x04 (IOIN), with W bit = 0 (read)
    spi1_transfer(0x00); // Dummy byte
    spi1_transfer(0x00); // Dummy byte
    spi1_transfer(0x00); // Dummy byte
    spi1_transfer(0x00); // Dummy byte
    spi1_cs_deselect();

    // A small delay between transactions is good practice
    delay(1000);

    // --- TRANSACTION 2: Clock out the requested data ---
    // The data we receive NOW is the content of the IOIN register.
    // We can send a "no-op" command, like reading GCONF (addr 0x00).
    spi1_cs_select();
    received_datagram[0] = spi1_transfer(0x00); // Send dummy command, receive STATUS byte
    received_datagram[1] = spi1_transfer(0x00); // Send dummy, receive IOIN MSB
    received_datagram[2] = spi1_transfer(0x00); // Send dummy, receive IOIN byte 2
    received_datagram[3] = spi1_transfer(0x00); // Send dummy, receive IOIN byte 1
    received_datagram[4] = spi1_transfer(0x00); // Send dummy, receive IOIN LSB
    spi1_cs_deselect();

    // Combine the bytes into a 32-bit value for easier inspection
    spi_status_byte = received_datagram[0];
    ioin_register_value = (received_datagram[1] << 24) |
                          (received_datagram[2] << 16) |
                          (received_datagram[3] << 8)  |
                          (received_datagram[4] << 0);

    // 3. Stop here for debugging
    while(1) {
        // Infinite loop to halt the processor
    }
    return 0;
}
