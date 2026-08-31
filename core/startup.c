#include "core/startup.h"

uint8_t blheli_s_startup_pwm_limit(uint8_t startup_power_decoded)
{
    uint16_t product = (uint16_t)50u * startup_power_decoded;

    return (uint8_t)(product >> 7u);
}

uint16_t blheli_s_startup_initial_commutation_period_four_x(void)
{
    return 0xf000u;
}

void blheli_s_startup_build_action_trace(
    struct blheli_s_startup_action_trace *trace, bool direction_reversed)
{
    *trace = (struct blheli_s_startup_action_trace){
        .action_count = 19u,
        .direction_reversed = direction_reversed,
        .requests_initial_temperature = true,
        .enters_startup_phase = true,
        .timing_initialization_count = 3u,
        .virtual_commutation_calculation_count = 2u,
        .actions = {
            { BLHELI_S_STARTUP_ACTION_DISABLE_INTERRUPTS, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_SWITCH_POWER_OFF, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_ENABLE_INTERRUPTS, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_CLEAR_RUNTIME_FLAGS, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_REQUEST_INITIAL_TEMPERATURE, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_CHECK_TEMPERATURE_POWER, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_DISABLE_INTERRUPTS, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_CALCULATE_STARTUP_PWM, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_RESTORE_PWM_LIMITS, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_ENABLE_INTERRUPTS, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_SELECT_DIRECTION, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_ENTER_STARTUP_PHASE, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_COMMUTATE,
              BLHELI_S_COMMUTATION_STEP_5,
              BLHELI_S_COMMUTATION_STEP_6 },
            { BLHELI_S_STARTUP_ACTION_COMMUTATE,
              BLHELI_S_COMMUTATION_STEP_6,
              BLHELI_S_COMMUTATION_STEP_1 },
            { BLHELI_S_STARTUP_ACTION_INITIALIZE_TIMING, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_CALCULATE_VIRTUAL_COMMUTATION, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_INITIALIZE_TIMING, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_CALCULATE_VIRTUAL_COMMUTATION, 0, 0 },
            { BLHELI_S_STARTUP_ACTION_INITIALIZE_TIMING, 0, 0 }
        }
    };
}

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
        .initial_commutation_period_four_x =
            blheli_s_startup_initial_commutation_period_four_x(),
        .initial_commutation_count = BLHELI_S_STARTUP_INITIAL_COMMUTATIONS,
        .initial_commutation_from = {
            BLHELI_S_COMMUTATION_STEP_5,
            BLHELI_S_COMMUTATION_STEP_6
        },
        .initial_commutation_to = {
            BLHELI_S_COMMUTATION_STEP_6,
            BLHELI_S_COMMUTATION_STEP_1
        },
        .timing_initialization_count = 3u,
        .virtual_commutation_calculation_count = 2u,
        .initial_temperature_requested = true,
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
