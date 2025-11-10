#include "stm32f4xx.h"
#include <rcc.h>
#include <gpio.h>

// Delay function (simple implementation)
void delay(volatile uint32_t count) {
    while(count--);
}

int main(void) {
    // 1. Configure system clock
    rcc_system_clock_config();

    // 2. Enable clock for GPIOD (where LEDs are on Discovery boards)
    rcc_gpio_port_clock_enable(GPIOD);

    // 3. Configure PD12 (Green LED) as an output
    gpio_configure_output_pin(GPIOD, 12);

    while(1) {
        // 4. Toggle the LED
        gpio_toggle_pin(GPIOD, 12);
        delay(1000000); // Simple delay
    }
    return 0;
}
