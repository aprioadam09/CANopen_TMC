#ifndef PERIPHERAL_INC_TMC5160_H_
#define PERIPHERAL_INC_TMC5160_H_

#include <stdint.h>

// TMC5160 Register Addresses
#define TMC5160_GCONF           0x00 // Global Configuration
#define TMC5160_GSTAT           0x01 // Global Status
#define TMC5160_IOIN            0x04 // Input/Output Status
#define TMC5160_IHOLD_IRUN      0x10 // Driver Current Control
#define TMC5160_TPOWERDOWN      0x11 // Standstill Delay
#define TMC5160_TPWMTHRS        0x13 // StealthChop voltage PWM mode
#define TMC5160_CHOPCONF        0x6C // Chopper Configuration

// Ramp Generator Registers
#define TMC5160_RAMPMODE        0x20 // Ramp Mode configuration
#define TMC5160_XACTUAL			0x21
#define TMC5160_V1              0x25 // First acceleration phase threshold speed
#define TMC5160_AMAX            0x26 // Acceleration
#define TMC5160_VMAX            0x27 // Maximum velocity
#define TMC5160_DMAX            0x28 // Deceleration
#define TMC5160_D1				0x2A // Deceleration between V1 and VSTOP
#define TMC5160_VSTOP			0x2B // Motor Stop Velocity
#define TMC5160_TZEROWAIT		0x2C
#define TMC5160_XTARGET         0x2D // Target Position
#define TMC5160_RAMP_STAT		0x35

/**
 * @brief Writes a 32-bit value to a TMC5160 register.
 *
 * This function handles the 40-bit SPI datagram protocol for a write operation.
 * The address's MSB is automatically set to 1 to indicate a write access.
 *
 * @param address The 7-bit register address (0x00 to 0x7F).
 * @param value The 32-bit data to write to the register.
 */
void tmc5160_write_register(uint8_t address, int32_t value);

/**
 * @brief Reads a 32-bit value from a TMC5160 register.
 *
 * This function handles the pipelined nature of TMC5160 SPI reads.
 * It performs two SPI transactions: one to request the data, and a second
 * to clock it out.
 *
 * @param address The 7-bit register address (0x00 to 0x7F).
 * @return int32_t The 32-bit value read from the register.
 */
int32_t tmc5160_read_register(uint8_t address);

/**
 * @brief Initializes the TMC5160 with a basic, proven configuration.
 *
 * This function configures the essential registers for motor operation, including
 * chopper settings (CHOPCONF), driver current (IHOLD_IRUN), and enables
 * StealthChop mode for quiet operation at low speeds.
 * It is based on the "Initialization Examples" from the TMC5160 datasheet.
 */
void tmc5160_init(void);


#endif /* PERIPHERAL_INC_TMC5160_H_ */
