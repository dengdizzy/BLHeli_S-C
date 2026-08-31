#ifndef BLHELI_S_TIMING_CONTROL_H
#define BLHELI_S_TIMING_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

struct blheli_s_timing_waits {
    uint16_t commutation;
    uint16_t advance;
    uint16_t zero_cross_scan;
    uint16_t zero_cross_timeout;
};

enum blheli_s_timing_timer_action {
    BLHELI_S_TIMING_TIMER_NONE,
    BLHELI_S_TIMING_TIMER_WAIT_ADVANCE,
    BLHELI_S_TIMING_TIMER_ARM_ZERO_CROSS_SCAN
};

struct blheli_s_timing_descriptor {
    uint32_t current_commutation;
    uint32_t previous_commutation;
    uint32_t previous_previous_commutation;
    uint16_t commutation_period_four_x;
    uint8_t programmed_commutation_timing;
    uint8_t demag_metric;
    bool clock_48mhz;
    bool startup_phase;
    bool initial_run_phase;
    bool high_rpm;
};

struct blheli_s_timing_update {
    uint32_t previous_commutation;
    uint32_t previous_previous_commutation;
    uint16_t commutation_period_four_x;
    struct blheli_s_timing_waits waits;
    uint8_t effective_commutation_timing;
    bool high_rpm;
    bool fast_path;
    bool overflow_clamped;
    bool minimum_wait_clamped;
    enum blheli_s_timing_timer_action timer_action;
};

uint8_t blheli_s_timing_effective_commutation_timing(
    uint8_t programmed_commutation_timing, uint8_t demag_metric,
    bool startup_phase);
void blheli_s_calculate_timing_waits(
    uint16_t commutation_period_four_x, uint8_t commutation_timing,
    bool clock_48mhz, bool startup, struct blheli_s_timing_waits *waits);
void blheli_s_timing_calculate_next(
    const struct blheli_s_timing_descriptor *descriptor,
    struct blheli_s_timing_update *update);

#endif
