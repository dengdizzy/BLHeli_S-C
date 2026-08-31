#include "core/run_control.h"

void blheli_s_run_begin(struct blheli_s_run_state *state,
                        uint8_t demag_power_off_threshold)
{
    *state = (struct blheli_s_run_state){
        .step = BLHELI_S_COMMUTATION_STEP_1,
        .demag_metric = 120u,
        .demag_power_off_threshold = demag_power_off_threshold
    };
}

enum blheli_s_run_result blheli_s_run_update(
    struct blheli_s_run_state *state, bool zero_cross_timeout,
    bool demag_detected, bool direction_change_brake)
{
    uint16_t metric = (uint16_t)state->demag_metric * 7u;

    if (demag_detected) {
        metric++;
    }
    state->demag_metric = (uint8_t)(metric >> 3u);
    if (state->demag_metric < 120u) {
        state->demag_metric = 120u;
    }

    if (zero_cross_timeout && !direction_change_brake &&
        !demag_detected) {
        return BLHELI_S_RUN_SYNC_LOST;
    }
    state->step = blheli_s_commutation_next_step(state->step);
    return BLHELI_S_RUN_COMMUTATE;
}
