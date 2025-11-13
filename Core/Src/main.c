#include "stm32f4xx.h"
#include "rcc.h"
#include "spi.h"
#include "tmc5160.h"

// Global volatile variables for easy debugging
volatile int32_t initial_xtarget_value = 0;
volatile int32_t value_to_write = 0x87654321;
volatile int32_t final_xtarget_value = 0;

void delay(volatile uint32_t count) {
    while(count--);
}

int main(void) {
    // 1. Configure system clock and initialize SPI
    rcc_system_clock_config();
    spi1_init();
    delay(1000000); // Wait for TMC5160 to power up

    // --- STEP A: Initial Read ---
    // Baca XTARGET. Setelah reset, ini harus 0.
    initial_xtarget_value = tmc5160_read_register(TMC5160_XTARGET);

    // --- STEP B: Write Operation ---
    // Tulis nilai 32-bit penuh ke XTARGET.
    tmc5160_write_register(TMC5160_XTARGET, value_to_write);

    delay(1000); // Delay singkat

    // --- STEP C: Final Read ---
    // Baca kembali XTARGET untuk verifikasi.
    final_xtarget_value = tmc5160_read_register(TMC5160_XTARGET);

    // --- STEP D: Stop for debugging ---
    while(1) {
        // Halt processor for inspection
    }
    return 0;
}
