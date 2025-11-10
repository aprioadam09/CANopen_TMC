#include <rcc.h>

void rcc_system_clock_config(void) {
    // 1. Enable HSE (High Speed External) oscillator
    RCC->CR |= RCC_CR_HSEON;
    // Wait until HSE is ready
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 2. Configure Flash and power settings for high speed
    // Enable instruction & data caches, prefetch buffer
    FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
    // Set Flash latency to 5 wait states for 168MHz
    FLASH->ACR |= FLASH_ACR_LATENCY_5WS;
    // Set voltage scaling
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    // 3. Configure PLL (Phase-Locked Loop)
    // Source = HSE, PLLM = 8, PLLN = 336, PLLP = 2, PLLQ = 7
    // HSE (8MHz) / PLLM(8) = 1MHz
    // 1MHz * PLLN(336) = 336MHz (VCO frequency)
    // 336MHz / PLLP(2) = 168MHz (System Clock)
    // 336MHz / PLLQ(7) = 48MHz (for USB, SDIO)
    RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSE) | (8 << RCC_PLLCFGR_PLLM_Pos)
                 | (336 << RCC_PLLCFGR_PLLN_Pos) | (0 << RCC_PLLCFGR_PLLP_Pos) // PLLP = 2 -> 00
                 | (7 << RCC_PLLCFGR_PLLQ_Pos);

    // 4. Enable PLL and wait for it to be ready
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 5. Configure AHB and APB bus prescalers
    // AHB Prescaler /1 (HCLK = 168MHz)
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    // APB2 Prescaler /2 (PCLK2 = 84MHz)
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
    // APB1 Prescaler /4 (PCLK1 = 42MHz)
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

    // 6. Select PLL as the system clock source
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    // Wait until PLL is used as the system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // Update the SystemCoreClock variable (optional but good practice)
    SystemCoreClockUpdate();
}

void rcc_gpio_port_clock_enable(GPIO_TypeDef* port) {
    if (port == GPIOA) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    } else if (port == GPIOB) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    } else if (port == GPIOC) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    } else if (port == GPIOD) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    } else if (port == GPIOE) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    }
    // Add other ports if needed
}
