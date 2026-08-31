#include <assert.h>

#include "core/brake.h"

void test_brake(void)
{
    struct blheli_s_brake_descriptor descriptor = {
        .bidirectional = false,
        .brake_on_stop = false,
        .requested_reverse = false,
        .actual_reverse = false,
        .direction_change_brake = false,
        .stop_count = 249u,
        .rcp_timeout_countdown = 1u,
        .commutation_period_four_x_high = 0x10u,
        .pwm_limit_begin = 42u
    };
    struct blheli_s_brake_decision decision;
    struct blheli_s_brake_state state;

    blheli_s_brake_begin(&state, true);
    assert(blheli_s_brake_update(&state, 2u, false, false, 0xf0u) ==
           BLHELI_S_BRAKE_RUN);
    assert(blheli_s_brake_update(&state, 3u, false, false, 0xf0u) ==
           BLHELI_S_BRAKE_STOP);
    assert(state.brake_active);

    blheli_s_brake_begin(&state, false);
    assert(blheli_s_brake_update(&state, 0u, true, false, 0x40u) ==
           BLHELI_S_BRAKE_REVERSE_START);
    assert(state.direction_change);
    assert(blheli_s_brake_update(&state, 0u, true, false, 0x20u) ==
           BLHELI_S_BRAKE_RUN);

    blheli_s_brake_evaluate(&descriptor, &decision);
    assert(decision.result == BLHELI_S_BRAKE_RUN);
    assert(decision.stop_threshold == 250u);
    assert(decision.trace.actions[0] ==
           BLHELI_S_BRAKE_ACTION_RESET_STALL_COUNT);

    descriptor.stop_count = 250u;
    blheli_s_brake_evaluate(&descriptor, &decision);
    assert(decision.result == BLHELI_S_BRAKE_STOP);
    assert(decision.stop_due_to_stop_count);
    assert(decision.switch_power_off);
    assert(decision.clears_runtime_flags);
    assert(decision.trace.action_count == 10u);
    assert(decision.trace.actions[1] ==
           BLHELI_S_BRAKE_ACTION_SWITCH_POWER_OFF_DISABLE_INTERRUPTS);
    assert(decision.trace.actions[2] ==
           BLHELI_S_BRAKE_ACTION_ALL_PWM_FETS_OFF);
    assert(decision.trace.actions[3] ==
           BLHELI_S_BRAKE_ACTION_ALL_COMMUTATION_FETS_OFF);
    assert(decision.trace.actions[4] ==
           BLHELI_S_BRAKE_ACTION_SET_PWMS_OFF);
    assert(decision.trace.actions[5] ==
           BLHELI_S_BRAKE_ACTION_WAIT_FOR_PWM_OFF);
    assert(decision.trace.actions[6] ==
           BLHELI_S_BRAKE_ACTION_SWITCH_POWER_OFF_DISABLE_INTERRUPTS);
    assert(decision.trace.actions[9] ==
           BLHELI_S_BRAKE_ACTION_SET_PWMS_OFF);

    descriptor.brake_on_stop = true;
    descriptor.stop_count = 3u;
    blheli_s_brake_evaluate(&descriptor, &decision);
    assert(decision.result == BLHELI_S_BRAKE_STOP);
    assert(decision.stop_threshold == 3u);
    assert(decision.brake_active);
    assert(decision.brake_on_stop_fets_on);
    assert(decision.trace.actions[10] ==
           BLHELI_S_BRAKE_ACTION_ALL_COMMUTATION_FETS_ON);

    descriptor.stop_count = 0u;
    descriptor.rcp_timeout_countdown = 0u;
    descriptor.brake_on_stop = false;
    blheli_s_brake_evaluate(&descriptor, &decision);
    assert(decision.result == BLHELI_S_BRAKE_STOP);
    assert(decision.stop_due_to_timeout);

    descriptor.rcp_timeout_countdown = 1u;
    descriptor.bidirectional = true;
    descriptor.requested_reverse = true;
    descriptor.actual_reverse = false;
    descriptor.commutation_period_four_x_high = 0x10u;
    blheli_s_brake_evaluate(&descriptor, &decision);
    assert(decision.result == BLHELI_S_BRAKE_REVERSE_START);
    assert(decision.direction_change_brake);
    assert(decision.brake_active);
    assert(decision.returns_to_run4);
    assert(decision.pwm_limit == 42u);
    assert(decision.trace.actions[1] ==
           BLHELI_S_BRAKE_ACTION_SET_DIRECTION_CHANGE_BRAKE);
    assert(decision.trace.actions[2] ==
           BLHELI_S_BRAKE_ACTION_RESTORE_PWM_LIMIT_BEGIN);
    assert(decision.trace.actions[3] ==
           BLHELI_S_BRAKE_ACTION_RETURN_TO_RUN4);

    descriptor.direction_change_brake = true;
    blheli_s_brake_evaluate(&descriptor, &decision);
    assert(decision.result == BLHELI_S_BRAKE_RUN);
    assert(decision.speed_threshold == 0x20u);

    descriptor.commutation_period_four_x_high = 0x20u;
    blheli_s_brake_evaluate(&descriptor, &decision);
    assert(decision.result == BLHELI_S_BRAKE_REVERSE_START);
    assert(!decision.direction_change_brake);
    assert(decision.actual_reverse);
    assert(decision.enters_initial_run);
    assert(decision.initial_run_rotations == 18u);
    assert(decision.trace.actions[1] ==
           BLHELI_S_BRAKE_ACTION_RESTORE_PWM_LIMIT_BEGIN);
    assert(decision.trace.actions[2] ==
           BLHELI_S_BRAKE_ACTION_CLEAR_DIRECTION_CHANGE_BRAKE);
    assert(decision.trace.actions[3] ==
           BLHELI_S_BRAKE_ACTION_SET_ACTUAL_DIRECTION);
    assert(decision.trace.actions[4] ==
           BLHELI_S_BRAKE_ACTION_ENTER_INITIAL_RUN);

    descriptor.bidirectional = false;
    descriptor.requested_reverse = false;
    descriptor.actual_reverse = false;
    descriptor.direction_change_brake = false;
    descriptor.stop_count = 0u;
    descriptor.commutation_period_four_x_high = 0xf0u;
    blheli_s_brake_evaluate(&descriptor, &decision);
    assert(decision.result == BLHELI_S_BRAKE_STOP);
    assert(decision.speed_threshold == 0xf0u);
    assert(decision.stop_due_to_speed);
}
