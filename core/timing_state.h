#ifndef BLHELI_S_TIMING_STATE_H
#define BLHELI_S_TIMING_STATE_H

#include <stdint.h>

struct blheli_s_timing_state {
    uint32_t previous_commutation;
    uint32_t previous_previous_commutation;
    uint16_t commutation_period_four_x;
    uint16_t advance_start;
    uint16_t zero_cross_scan_start;
    uint16_t zero_cross_timeout_start;
    uint16_t commutation_wait_start;
    uint32_t timer0_extended;
    uint32_t timer2_extended;
};

#endif
