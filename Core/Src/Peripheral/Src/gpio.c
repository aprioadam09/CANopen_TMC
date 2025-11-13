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

void gpio_configure_alternate_function(GPIO_TypeDef* port, uint8_t pin_number, uint8_t af_selection) {
    // Set pin mode to Alternate Function (10)
    port->MODER &= ~(0x3 << (pin_number * 2)); // Clear bits
    port->MODER |= (0x2 << (pin_number * 2));  // Set to 10

    // Set speed to very high speed (11) for peripherals like SPI
    port->OSPEEDR &= ~(0x3 << (pin_number * 2));
    port->OSPEEDR |= (0x3 << (pin_number * 2));

    // Set to No pull-up, pull-down (00)
    port->PUPDR &= ~(0x3 << (pin_number * 2));

    // Select the alternate function
    if (pin_number < 8) {
        // Use the lower alternate function register (AFRL)
        uint32_t shift = pin_number * 4;
        port->AFR[0] &= ~(0xF << shift); // Clear the 4 bits for the pin
        port->AFR[0] |= (af_selection << shift); // Set the AF selection
    } else {
        // Use the higher alternate function register (AFRH)
        uint32_t shift = (pin_number - 8) * 4;
        port->AFR[1] &= ~(0xF << shift); // Clear the 4 bits for the pin
        port->AFR[1] |= (af_selection << shift); // Set the AF selection
    }
}

void gpio_toggle_pin(GPIO_TypeDef* port, uint8_t pin_number) {
    port->ODR ^= (1 << pin_number);
}
