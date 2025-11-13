#ifndef PERIPHERAL_INC_SPI_H_
#define PERIPHERAL_INC_SPI_H_

#include "stm32f4xx.h"
#include <stdint.h>

/**
 * @brief Configures GPIO pins for SPI1 and initializes the SPI1 peripheral.
 *
 * This function performs the following configurations:
 * 1. Enables the clock for GPIOA.
 * 2. Configures PA5 (SCK), PA6 (MISO), and PA7 (MOSI) for Alternate Function 5 (SPI1).
 * 3. Configures PA4 (CSN) as a general-purpose output pin for software-controlled Chip Select.
 * 4. Enables the clock for the SPI1 peripheral.
 * 5. Configures SPI1 settings:
 *    - SPI Mode 3 (CPOL=1, CPHA=1)
 *    - Master mode
 *    - 8-bit data frame format
 *    - MSB first
 *    - Software slave management enabled (SSI and SSM bits set)
 *    - Baud rate set to fPCLK/128 (approx. 42MHz / 128 = 328 KHz, a safe starting speed)
 * 6. Enables the SPI1 peripheral.
 */
void spi1_init(void);

/**
 * @brief Selects the SPI slave device by pulling its CS pin low.
 *
 * For our project, this pulls PA4 low.
 */
void spi1_cs_select(void);

/**
 * @brief Deselects the SPI slave device by pulling its CS pin high.
 *
 * For our project, this pulls PA4 high.
 */
void spi1_cs_deselect(void);

/**
 * @brief Transmits and receives one byte of data via SPI1.
 *
 * @param data The byte to be transmitted.
 * @return uint8_t The byte received from the slave device.
 */
uint8_t spi1_transfer(uint8_t data);


#endif /* PERIPHERAL_INC_SPI_H_ */
