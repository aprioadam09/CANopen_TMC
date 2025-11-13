#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f4xx.h"

/**
 * @brief Configures a GPIO pin as a standard push-pull output.
 * @param port Pointer to the GPIO_TypeDef for the port (e.g., GPIOD).
 * @param pin_number The pin number (0-15).
 */
void gpio_configure_output_pin(GPIO_TypeDef* port, uint8_t pin_number);

/**
 * @brief Configures a GPIO pin for an alternate function.
 * @param port Pointer to the GPIO_TypeDef for the port (e.g., GPIOA).
 * @param pin_number The pin number (0-15).
 * @param af_selection The alternate function selection (0-15), e.g., 5 for AF5 (SPI1).
 */
void gpio_configure_alternate_function(GPIO_TypeDef* port, uint8_t pin_number, uint8_t af_selection);

/**
 * @brief Toggles the state of a GPIO output pin.
 * @param port Pointer to the GPIO_TypeDef for the port.
 * @param pin_number The pin number (0-15).
 */
void gpio_toggle_pin(GPIO_TypeDef* port, uint8_t pin_number);

#endif /* GPIO_H_ */
