#ifndef RCC_H_
#define RCC_H_

#include "stm32f4xx.h" // CMSIS header for register definitions

/**
 * @brief Configures the system clock to 168MHz using HSE and PLL.
 * This function must be called first after reset.
 */
void rcc_system_clock_config(void);

/**
 * @brief Enables the clock for a specific GPIO port.
 * @param port Pointer to the GPIO_TypeDef for the port (e.g., GPIOA, GPIOB).
 */
void rcc_gpio_port_clock_enable(GPIO_TypeDef* port);

#endif /* RCC_H_ */
