#include "core/startup.h"

void blheli_s_startup_begin(struct blheli_s_startup_state *state,
                            uint8_t startup_pwm_limit,
                            bool direction_reversed)
{
    *state = (struct blheli_s_startup_state){
        .commutation_count = 0u,
        .initial_run_rotations_remaining = 0u,
        .pwm_limit_begin = startup_pwm_limit,
        .pwm_limit = startup_pwm_limit,
        .pwm_limit_by_rpm = startup_pwm_limit,
        .direction_reversed = direction_reversed
    };
}

enum blheli_s_startup_result blheli_s_startup_after_commutation(
    struct blheli_s_startup_state *state, uint8_t throttle)
{
    if (throttle == 0u) {
        return BLHELI_S_STARTUP_STOP;
    }
    if (state->commutation_count < 24u) {
        state->commutation_count++;
    }
    if (state->commutation_count == 24u) {
        state->initial_run_rotations_remaining = 12u;
        state->pwm_limit = state->pwm_limit_begin;
        state->pwm_limit_by_rpm = state->pwm_limit_begin;
        return BLHELI_S_STARTUP_INITIAL_RUN;
    }
    return BLHELI_S_STARTUP_CONTINUE;
}

enum blheli_s_startup_result blheli_s_startup_after_rotation(
    struct blheli_s_startup_state *state)
{
    if (state->initial_run_rotations_remaining == 0u) {
        return BLHELI_S_STARTUP_CONTINUE;
    }
    state->initial_run_rotations_remaining--;
    return state->initial_run_rotations_remaining == 0u
        ? BLHELI_S_STARTUP_CONTINUE : BLHELI_S_STARTUP_INITIAL_RUN;
}
