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

void tmc5160_init(void) {
    // This sequence is based on the TMC5160 Datasheet Section 23.1 Initialization Examples.
    // It configures the driver for SpreadCycle and enables StealthChop below a certain speed.

    // 1. Configure Chopper (CHOPCONF Register)
    // A known-good starting value for many motors.
    // TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (SpreadCycle)
    tmc5160_write_register(TMC5160_CHOPCONF, 0x000100C3);

    // 2. Configure Driver Current (IHOLD_IRUN Register)
    // Sets the run current to the maximum (31/31) and the hold current to a lower value.
    // IRUN = 31 (max current)
    // IHOLD = 10 (approx 1/3 of max current)
    // IHOLDDELAY = 6 (delay before current reduction)
    tmc5160_write_register(TMC5160_IHOLD_IRUN, 0x00061F0A);

    // 3. Configure Standstill Power-Down Delay (TPOWERDOWN Register)
    // Sets the delay after standstill before the driver reduces current to IHOLD.
    // TPOWERDOWN = 10 (approx. 0.8 seconds with 12MHz clock)
    tmc5160_write_register(TMC5160_TPOWERDOWN, 0x0000000A);

    // 4. Enable StealthChop (GCONF Register)
    // Set en_pwm_mode (bit 2) to 1. This enables voltage-PWM mode (StealthChop).
    tmc5160_write_register(TMC5160_GCONF, 0x00000004);

    // 5. Set StealthChop threshold speed (TPWMTHRS Register)
    // The driver will switch from StealthChop to SpreadCycle when speed exceeds this value.
    // TPWM_THRS = 500 (a low speed value)
    tmc5160_write_register(TMC5160_TPWMTHRS, 0x000001F4);
}
