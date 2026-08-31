#include "core/esc_control.h"

void blheli_s_core_init(struct blheli_s_core_state *state)
{
    blheli_s_core_reset(state);
    state->initialized = true;
}

void blheli_s_core_reset(struct blheli_s_core_state *state)
{
    *state = (struct blheli_s_core_state){
        .phase = BLHELI_S_CONTROL_STOPPED,
        .initialized = false,
        .reserved = 0u
    };
}

void blheli_s_core_begin_startup(struct blheli_s_core_state *state,
                                 uint8_t startup_pwm_limit,
                                 bool direction_reversed,
                                 uint8_t demag_power_off_threshold)
{
    blheli_s_startup_begin(&state->startup, startup_pwm_limit,
                           direction_reversed);
    blheli_s_run_begin(&state->run, demag_power_off_threshold);
    state->motor.startup_count = 0u;
    state->motor.initial_run_rotation_countdown = 0u;
    state->motor.power_pwm_limit_begin = startup_pwm_limit;
    state->motor.power_pwm_limit = startup_pwm_limit;
    state->motor.power_pwm_limit_by_rpm = startup_pwm_limit;
    state->motor.direction_reversed = direction_reversed;
    state->motor.direction_change_brake = false;
    state->motor.motor_started = false;
    state->phase = BLHELI_S_CONTROL_STARTUP;
}

enum blheli_s_core_update_result blheli_s_core_after_run_event(
    struct blheli_s_core_state *state,
    const struct blheli_s_core_run_event *event)
{
    enum blheli_s_run_result run_result;
    enum blheli_s_startup_result startup_result;

    if (state->phase != BLHELI_S_CONTROL_STARTUP &&
        state->phase != BLHELI_S_CONTROL_INITIAL_RUN &&
        state->phase != BLHELI_S_CONTROL_RUNNING) {
        return BLHELI_S_CORE_WAITING;
    }

    run_result = blheli_s_run_update(&state->run, event->zero_cross_timeout,
                                     event->demag_detected,
                                     event->direction_change_brake);
    if (run_result == BLHELI_S_RUN_SYNC_LOST) {
        state->phase = BLHELI_S_CONTROL_STOPPED;
        state->motor.motor_started = false;
        if (event->throttle != 0u) {
            state->motor.stall_count++;
        }
        return BLHELI_S_CORE_SYNC_LOST;
    }

    if (state->phase == BLHELI_S_CONTROL_STARTUP) {
        startup_result = blheli_s_startup_after_commutation(&state->startup,
                                                            event->throttle);
        state->motor.startup_count = state->startup.commutation_count;
        state->motor.power_pwm_limit = state->startup.pwm_limit;
        state->motor.power_pwm_limit_by_rpm = state->startup.pwm_limit_by_rpm;
        if (startup_result == BLHELI_S_STARTUP_STOP) {
            state->phase = BLHELI_S_CONTROL_STOPPED;
            return BLHELI_S_CORE_STOPPED;
        }
        if (startup_result == BLHELI_S_STARTUP_INITIAL_RUN) {
            state->phase = BLHELI_S_CONTROL_INITIAL_RUN;
            state->motor.initial_run_rotation_countdown =
                state->startup.initial_run_rotations_remaining;
        }
    }

    if (state->phase == BLHELI_S_CONTROL_INITIAL_RUN &&
        state->run.step == BLHELI_S_COMMUTATION_STEP_1) {
        (void)blheli_s_startup_after_rotation(&state->startup);
        state->motor.initial_run_rotation_countdown =
            state->startup.initial_run_rotations_remaining;
        if (state->startup.initial_run_rotations_remaining == 0u) {
            state->phase = BLHELI_S_CONTROL_RUNNING;
            state->motor.motor_started = true;
            return BLHELI_S_CORE_RUNNING;
        }
        return BLHELI_S_CORE_INITIAL_RUN;
    }

    return BLHELI_S_CORE_COMMUTATE;
}
