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

void blheli_s_calculate_timing_waits(
    uint16_t commutation_period_four_x, uint8_t commutation_timing,
    bool clock_48mhz, bool startup, struct blheli_s_timing_waits *waits);

#endif
