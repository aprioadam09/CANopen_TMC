#include "spi.h"
#include "rcc.h"
#include "gpio.h"

void spi1_init(void) {
    // 1. Enable peripheral clocks
    rcc_gpio_port_clock_enable(GPIOA);
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // 2. Configure GPIO pins using our GPIO driver
    // AF5 is the alternate function for SPI1 on STM32F407
    gpio_configure_alternate_function(GPIOA, 5, 5); // PA5 (SCK)
    gpio_configure_alternate_function(GPIOA, 6, 5); // PA6 (MISO)
    gpio_configure_alternate_function(GPIOA, 7, 5); // PA7 (MOSI)

    // 3. Configure CSN pin (PA4) as a standard output
    gpio_configure_output_pin(GPIOA, 4);
    spi1_cs_deselect(); // Initialize CS pin to high (deselected)

    // 4. Configure the SPI1 peripheral
    SPI1->CR1 &= ~SPI_CR1_SPE; // Ensure peripheral is disabled

    SPI1->CR1 = 0; // Clear CR1 register to a known state

    // Set Baud rate to fPCLK2/64 (84MHz / 64 = 1.3125 MHz)
    // BR[2:0] = 101
    SPI1->CR1 |= (0x5 << SPI_CR1_BR_Pos);

    // Set CPOL=1, CPHA=1 (SPI Mode 3)
    SPI1->CR1 |= SPI_CR1_CPOL | SPI_CR1_CPHA;

    // Set to Master mode, enable Software Slave Management
    SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;

    // Default settings are correct for: 8-bit data, MSB first.

    // 5. Enable the SPI1 peripheral
    SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_cs_select(void) {
    // Atomically set PA4 low using the Bit Set/Reset Register (BSRR)
    // The upper 16 bits of BSRR are for resetting bits
    GPIOA->BSRR = (1 << (4 + 16));
}

void spi1_cs_deselect(void) {
    // Atomically set PA4 high using the Bit Set/Reset Register (BSRR)
    // The lower 16 bits of BSRR are for setting bits
    GPIOA->BSRR = (1 << 4);
}

uint8_t spi1_transfer(uint8_t data) {
    // Wait until the Transmit buffer is empty (TXE flag is set)
    while (!(SPI1->SR & SPI_SR_TXE));

    // Write the data to the Data Register to start transmission
    SPI1->DR = data;

    // Wait until the Receive buffer is not empty (RXNE flag is set)
    while (!(SPI1->SR & SPI_SR_RXNE));

    // Read and return the received data from the Data Register
    // This also clears the RXNE flag
    return SPI1->DR;
}
