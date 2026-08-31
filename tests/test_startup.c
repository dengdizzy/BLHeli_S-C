#include <assert.h>

#include "config/blheli_s_config.h"
#include "core/startup.h"

void test_startup(void)
{
    struct blheli_s_config config;
    struct blheli_s_decoded_config decoded;
    struct blheli_s_startup_state state;
    struct blheli_s_startup_action_trace trace;

    blheli_s_config_defaults(&config);
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.startup_power_decoded == 0x40u);
    assert(blheli_s_startup_pwm_limit(0x04u) == 1u);
    assert(blheli_s_startup_pwm_limit(decoded.startup_power_decoded) == 25u);
    assert(blheli_s_startup_pwm_limit(0xc0u) == 75u);
    assert(blheli_s_startup_initial_commutation_period_four_x() == 0xf000u);

    blheli_s_startup_build_action_trace(&trace, true);
    assert(trace.action_count == 19u);
    assert(trace.direction_reversed);
    assert(trace.requests_initial_temperature);
    assert(trace.enters_startup_phase);
    assert(trace.timing_initialization_count == 3u);
    assert(trace.virtual_commutation_calculation_count == 2u);
    assert(trace.actions[0].kind ==
           BLHELI_S_STARTUP_ACTION_DISABLE_INTERRUPTS);
    assert(trace.actions[1].kind ==
           BLHELI_S_STARTUP_ACTION_SWITCH_POWER_OFF);
    assert(trace.actions[2].kind ==
           BLHELI_S_STARTUP_ACTION_ENABLE_INTERRUPTS);
    assert(trace.actions[4].kind ==
           BLHELI_S_STARTUP_ACTION_REQUEST_INITIAL_TEMPERATURE);
    assert(trace.actions[7].kind ==
           BLHELI_S_STARTUP_ACTION_CALCULATE_STARTUP_PWM);
    assert(trace.actions[8].kind ==
           BLHELI_S_STARTUP_ACTION_RESTORE_PWM_LIMITS);
    assert(trace.actions[12].kind == BLHELI_S_STARTUP_ACTION_COMMUTATE);
    assert(trace.actions[12].from_step == BLHELI_S_COMMUTATION_STEP_5);
    assert(trace.actions[12].to_step == BLHELI_S_COMMUTATION_STEP_6);
    assert(trace.actions[13].kind == BLHELI_S_STARTUP_ACTION_COMMUTATE);
    assert(trace.actions[13].from_step == BLHELI_S_COMMUTATION_STEP_6);
    assert(trace.actions[13].to_step == BLHELI_S_COMMUTATION_STEP_1);
    assert(trace.actions[14].kind ==
           BLHELI_S_STARTUP_ACTION_INITIALIZE_TIMING);
    assert(trace.actions[15].kind ==
           BLHELI_S_STARTUP_ACTION_CALCULATE_VIRTUAL_COMMUTATION);
    assert(trace.actions[18].kind ==
           BLHELI_S_STARTUP_ACTION_INITIALIZE_TIMING);

    blheli_s_startup_begin(&state, 42u, true);
    assert(state.pwm_limit == 42u);
    assert(state.pwm_limit_by_rpm == 42u);
    assert(state.pwm_limit_begin == 42u);
    assert(state.initial_temperature_requested);
    assert(state.direction_reversed);
    assert(state.initial_commutation_period_four_x == 0xf000u);
    assert(state.initial_commutation_count == 2u);
    assert(state.initial_commutation_from[0] == BLHELI_S_COMMUTATION_STEP_5);
    assert(state.initial_commutation_to[0] == BLHELI_S_COMMUTATION_STEP_6);
    assert(state.initial_commutation_from[1] == BLHELI_S_COMMUTATION_STEP_6);
    assert(state.initial_commutation_to[1] == BLHELI_S_COMMUTATION_STEP_1);
    assert(state.timing_initialization_count == 3u);
    assert(state.virtual_commutation_calculation_count == 2u);
    assert(blheli_s_startup_after_commutation(&state, 1u) ==
           BLHELI_S_STARTUP_CONTINUE);
    for (unsigned int count = 1u; count < 24u; ++count) {
        assert(blheli_s_startup_after_commutation(&state, 1u) ==
               (count == 23u ? BLHELI_S_STARTUP_INITIAL_RUN
                             : BLHELI_S_STARTUP_CONTINUE));
    }
    assert(state.commutation_count == 24u);
    assert(state.initial_run_rotations_remaining == 12u);
    for (unsigned int count = 0u; count < 11u; ++count) {
        assert(blheli_s_startup_after_rotation(&state) ==
               BLHELI_S_STARTUP_INITIAL_RUN);
    }
    assert(blheli_s_startup_after_rotation(&state) ==
           BLHELI_S_STARTUP_CONTINUE);
    assert(state.initial_run_rotations_remaining == 0u);

    assert(blheli_s_startup_after_commutation(&state, 0u) ==
           BLHELI_S_STARTUP_STOP);
}
