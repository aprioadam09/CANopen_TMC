#include "systick.h"
#include "stm32f4xx.h"

// Volatile variable to hold the millisecond count
static volatile uint32_t ms_ticks = 0;

void systick_init(void) {
    // Configure SysTick for a 1ms interrupt.
    // The Core clock (HCLK) is 168MHz.
    // We need 168,000 ticks for 1ms.
    SysTick->LOAD = 168000 - 1; // Set reload register

    // Set priority for SysTick interrupt
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    // Reset the counter
    SysTick->VAL = 0;

    // Enable the SysTick interrupt and the timer itself
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
}

uint32_t millis(void) {
    return ms_ticks;
}

// SysTick Interrupt Service Routine
void SysTick_Handler(void) {
    ms_ticks++;
}
