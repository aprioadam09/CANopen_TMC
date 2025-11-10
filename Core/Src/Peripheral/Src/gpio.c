#include <gpio.h>

void gpio_configure_output_pin(GPIO_TypeDef* port, uint8_t pin_number) {
    // Set pin mode to General Purpose Output (01)
    port->MODER &= ~(0x3 << (pin_number * 2)); // Clear bits
    port->MODER |= (0x1 << (pin_number * 2));  // Set to 01

    // Set output type to Push-Pull (0)
    port->OTYPER &= ~(1 << pin_number);

    // Set speed to Low speed (00)
    port->OSPEEDR &= ~(0x3 << (pin_number * 2));

    // Set to No pull-up, pull-down (00)
    port->PUPDR &= ~(0x3 << (pin_number * 2));
}

void gpio_toggle_pin(GPIO_TypeDef* port, uint8_t pin_number) {
    port->ODR ^= (1 << pin_number);
}
