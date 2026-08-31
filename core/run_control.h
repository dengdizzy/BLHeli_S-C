#ifndef BLHELI_S_RUN_CONTROL_H
#define BLHELI_S_RUN_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

#include "core/commutation.h"

enum blheli_s_run_result {
    BLHELI_S_RUN_WAITING,
    BLHELI_S_RUN_COMMUTATE,
    BLHELI_S_RUN_SYNC_LOST
};

struct blheli_s_run_state {
    enum blheli_s_commutation_step step;
    uint8_t demag_metric;
    uint8_t demag_power_off_threshold;
};

void blheli_s_run_begin(struct blheli_s_run_state *state,
                        uint8_t demag_power_off_threshold);
enum blheli_s_run_result blheli_s_run_update(
    struct blheli_s_run_state *state, bool zero_cross_timeout,
    bool demag_detected, bool direction_change_brake);

#endif
