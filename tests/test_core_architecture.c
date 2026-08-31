#include <assert.h>

#include "core/esc_control.h"

void test_core_architecture(void)
{
    struct blheli_s_core_state state;
    const struct blheli_s_core_run_event run_event = {
        .throttle = 1u,
        .zero_cross_timeout = false,
        .demag_detected = false,
        .direction_change_brake = false
    };
    const struct blheli_s_core_run_event timed_out_event = {
        .throttle = 1u,
        .zero_cross_timeout = true,
        .demag_detected = false,
        .direction_change_brake = false
    };
    unsigned int index;

    blheli_s_core_init(&state);
    assert(state.initialized);
    assert(state.phase == BLHELI_S_CONTROL_STOPPED);
    assert(state.reserved == 0u);

    state.phase = BLHELI_S_CONTROL_RUNNING;
    blheli_s_core_reset(&state);
    assert(!state.initialized);
    assert(state.phase == BLHELI_S_CONTROL_STOPPED);

    blheli_s_core_init(&state);
    blheli_s_core_begin_startup(&state, 42u, true, 160u);
    assert(state.phase == BLHELI_S_CONTROL_STARTUP);
    assert(state.motor.power_pwm_limit == 42u);
    assert(state.motor.direction_reversed);
    for (index = 0u; index < 23u; index++) {
        assert(blheli_s_core_after_run_event(&state, &run_event) ==
               BLHELI_S_CORE_COMMUTATE);
    }
    assert(blheli_s_core_after_run_event(&state, &run_event) ==
           BLHELI_S_CORE_INITIAL_RUN);
    assert(state.motor.startup_count == 24u);
    assert(state.motor.initial_run_rotation_countdown == 11u);

    for (index = 0u; index < 65u; index++) {
        assert(blheli_s_core_after_run_event(&state, &run_event) ==
               BLHELI_S_CORE_COMMUTATE ||
               state.phase == BLHELI_S_CONTROL_INITIAL_RUN);
    }
    assert(blheli_s_core_after_run_event(&state, &run_event) ==
           BLHELI_S_CORE_RUNNING);
    assert(state.phase == BLHELI_S_CONTROL_RUNNING);
    assert(state.motor.motor_started);

    blheli_s_core_begin_startup(&state, 42u, false, 160u);
    assert(blheli_s_core_after_run_event(&state, &timed_out_event) ==
           BLHELI_S_CORE_SYNC_LOST);
    assert(state.phase == BLHELI_S_CONTROL_STOPPED);
    assert(state.motor.stall_count == 1u);
}
