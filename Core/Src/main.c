#include "stm32f4xx.h"
#include "rcc.h"
#include "spi.h"
#include "tmc5160.h"

// Simple delay function for a visual pause
void delay(volatile uint32_t count) {
    while(count--);
}

/**
 * @brief Polls the RAMP_STAT register until the motor has reached its target position,
 *        then clears the event flag.
 */
void wait_target_reached(void) {
    const int32_t POSITION_REACHED_FLAG = (1 << 9);
    const int32_t EVENT_POS_REACHED_CLEAR = (1 << 7); // Bit 7 is the event flag

    // 1. Poll the register until the position_reached flag is active
    while ((tmc5160_read_register(TMC5160_RAMP_STAT) & POSITION_REACHED_FLAG) == 0);

    // 2. Clear the event_pos_reached flag (R+WC) to allow detection for the next move.
    tmc5160_write_register(TMC5160_RAMP_STAT, EVENT_POS_REACHED_CLEAR);
}

int main(void) {
    // --- Initialization ---
    rcc_system_clock_config();
    spi1_init();
    tmc5160_init();

    // --- Motion Profile Configuration ---
    tmc5160_write_register(TMC5160_V1, 0);
    tmc5160_write_register(TMC5160_AMAX, 1000);
    tmc5160_write_register(TMC5160_DMAX, 1000);
    tmc5160_write_register(TMC5160_D1, 1000);
    tmc5160_write_register(TMC5160_VMAX, 51200);
    tmc5160_write_register(TMC5160_VSTOP, 100);

    // Add a zero-wait time for smooth direction reversals
    tmc5160_write_register(TMC5160_TZEROWAIT, 5000);

    // Set RAMPMODE to Positioning Mode
    tmc5160_write_register(TMC5160_RAMPMODE, 0);

    // --- Repetitive Motion Trigger ---
    while(1) {
        // Move to position 51200 (1 full revolution)
        tmc5160_write_register(TMC5160_XTARGET, 51200);

        // [FIX PER NOTEBOOK SUGGESTION] Wait for the move to complete using hardware status
        wait_target_reached();
        delay(1000000); // Add a short visual pause between moves

        // Move back to position 0
        tmc5160_write_register(TMC5160_XTARGET, 0);

        // [FIX PER NOTEBOOK SUGGESTION] Wait for the move to complete
        wait_target_reached();
        delay(1000000); // Add a short visual pause
    }
    return 0;
}
