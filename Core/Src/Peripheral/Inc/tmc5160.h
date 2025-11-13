#ifndef PERIPHERAL_INC_TMC5160_H_
#define PERIPHERAL_INC_TMC5160_H_

#include <stdint.h>

// TMC5160 Register Addresses
#define TMC5160_GCONF           0x00 // Global Configuration
#define TMC5160_GSTAT           0x01 // Global Status
#define TMC5160_IOIN            0x04 // Input/Output Status
#define TMC5160_IHOLD_IRUN      0x10 // Driver Current Control
#define TMC5160_TPOWERDOWN      0x11 // Standstill Delay
#define TMC5160_CHOPCONF        0x6C // Chopper Configuration
#define TMC5160_XTARGET         0x2D // Target Position

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


#endif /* PERIPHERAL_INC_TMC5160_H_ */
