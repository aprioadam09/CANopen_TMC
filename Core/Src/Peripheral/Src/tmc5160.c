#include "tmc5160.h"
#include "spi.h" // We depend on the SPI driver for communication

void tmc5160_write_register(uint8_t address, int32_t value) {
    uint8_t address_byte = address | 0x80;
    volatile uint8_t dummy_read; // Used to clear the RXNE flag

    spi1_cs_select();

    // Send address byte and clear the buffer
    dummy_read = spi1_transfer(address_byte);

    // Send the 32-bit value, MSB first, and clear the buffer each time
    dummy_read = spi1_transfer((value >> 24) & 0xFF);
    dummy_read = spi1_transfer((value >> 16) & 0xFF);
    dummy_read = spi1_transfer((value >> 8)  & 0xFF);
    dummy_read = spi1_transfer(value & 0xFF);

    (void)dummy_read; // Suppress "unused variable" warning

    spi1_cs_deselect();
}

int32_t tmc5160_read_register(uint8_t address) {
    uint8_t address_byte = address & 0x7F;
    int32_t received_value = 0;
    volatile uint8_t dummy_read;

    // --- Transaction 1: Request the register data ---
    spi1_cs_select();
    dummy_read = spi1_transfer(address_byte);
    dummy_read = spi1_transfer(0x00);
    dummy_read = spi1_transfer(0x00);
    dummy_read = spi1_transfer(0x00);
    dummy_read = spi1_transfer(0x00);
    spi1_cs_deselect();

    for (volatile int i = 0; i < 100; i++); // Delay singkat

    // --- Transaction 2: Clock out the requested data ---
    spi1_cs_select();
    dummy_read = spi1_transfer(address_byte);
    received_value |= ((int32_t)spi1_transfer(0x00) << 24);
    received_value |= ((int32_t)spi1_transfer(0x00) << 16);
    received_value |= ((int32_t)spi1_transfer(0x00) << 8);
    received_value |= (int32_t)spi1_transfer(0x00);
    spi1_cs_deselect();

    (void)dummy_read; // Hindari warning "unused variable"

    return received_value;
}
