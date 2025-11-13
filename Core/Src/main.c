#include "stm32f4xx.h"
#include "rcc.h"
#include "spi.h"
#include "tmc5160.h"

// Global volatile variable for easy debugging
volatile int32_t chopconf_value_after_init = 0;

int main(void) {
    // 1. Basic hardware initialization
    rcc_system_clock_config();
    spi1_init();

    // 2. Initialize the TMC5160 with our default settings
    tmc5160_init();

    // 3. Read back a register to verify the initialization
    chopconf_value_after_init = tmc5160_read_register(TMC5160_CHOPCONF);

    // 4. Stop here for debugging
    while(1) {
        // Halt processor for inspection
    }
    return 0;
}
