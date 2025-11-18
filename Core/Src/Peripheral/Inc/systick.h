#ifndef PERIPHERAL_INC_SYSTICK_H_
#define PERIPHERAL_INC_SYSTICK_H_

#include <stdint.h>

/**
 * @brief Initializes the SysTick timer to generate an interrupt every 1 millisecond.
 */
void systick_init(void);

/**
 * @brief Returns the number of milliseconds elapsed since systick_init() was called.
 * @return Milliseconds as a 32-bit unsigned integer.
 */
uint32_t millis(void);

#endif /* PERIPHERAL_INC_SYSTICK_H_ */
