#include "core/brake.h"

static void append_brake_action(struct blheli_s_brake_action_trace *trace,
                                enum blheli_s_brake_action_kind action)
{
    trace->actions[trace->action_count++] = action;
}

static void append_switch_power_off(struct blheli_s_brake_decision *decision)
{
    decision->switch_power_off = true;
    append_brake_action(&decision->trace,
                        BLHELI_S_BRAKE_ACTION_SWITCH_POWER_OFF_DISABLE_INTERRUPTS);
    append_brake_action(&decision->trace,
                        BLHELI_S_BRAKE_ACTION_ALL_PWM_FETS_OFF);
    append_brake_action(&decision->trace,
                        BLHELI_S_BRAKE_ACTION_ALL_COMMUTATION_FETS_OFF);
    append_brake_action(&decision->trace,
                        BLHELI_S_BRAKE_ACTION_SET_PWMS_OFF);
}

void blheli_s_brake_begin(struct blheli_s_brake_state *state,
                          bool brake_on_stop)
{
    *state = (struct blheli_s_brake_state){
        .direction_change = false,
        .brake_on_stop = brake_on_stop,
        .brake_active = false,
        .stop_count = 0u,
        .speed_period = 0u
    };
}

void blheli_s_brake_evaluate(const struct blheli_s_brake_descriptor *descriptor,
                             struct blheli_s_brake_decision *decision)
{
    bool direction_mismatch =
        descriptor->requested_reverse != descriptor->actual_reverse;

    *decision = (struct blheli_s_brake_decision){
        .result = BLHELI_S_BRAKE_RUN,
        .stop_threshold = descriptor->brake_on_stop ? 3u : 250u,
        .speed_threshold = descriptor->direction_change_brake ? 0x20u : 0xf0u,
        .pwm_limit = descriptor->pwm_limit_begin,
        .direction_change_brake = descriptor->direction_change_brake,
        .actual_reverse = descriptor->actual_reverse,
        .brake_active = descriptor->direction_change_brake
    };

    append_brake_action(&decision->trace,
                        BLHELI_S_BRAKE_ACTION_RESET_STALL_COUNT);

    if (!descriptor->bidirectional &&
        descriptor->stop_count >= decision->stop_threshold) {
        decision->result = BLHELI_S_BRAKE_STOP;
        decision->stop_due_to_stop_count = true;
        decision->brake_active = descriptor->brake_on_stop;
        append_switch_power_off(decision);
        append_brake_action(&decision->trace,
                            BLHELI_S_BRAKE_ACTION_WAIT_FOR_PWM_OFF);
        append_switch_power_off(decision);
        if (descriptor->brake_on_stop) {
            decision->brake_on_stop_fets_on = true;
            append_brake_action(&decision->trace,
                                BLHELI_S_BRAKE_ACTION_ALL_COMMUTATION_FETS_ON);
        }
        decision->clears_runtime_flags = true;
        return;
    }

    if (descriptor->rcp_timeout_countdown == 0u) {
        decision->result = BLHELI_S_BRAKE_STOP;
        decision->stop_due_to_timeout = true;
        append_switch_power_off(decision);
        append_brake_action(&decision->trace,
                            BLHELI_S_BRAKE_ACTION_WAIT_FOR_PWM_OFF);
        append_switch_power_off(decision);
        decision->clears_runtime_flags = true;
        return;
    }

    if (descriptor->bidirectional && direction_mismatch &&
        !descriptor->direction_change_brake) {
        decision->result = BLHELI_S_BRAKE_REVERSE_START;
        decision->direction_change_brake = true;
        decision->brake_active = true;
        decision->returns_to_run4 = true;
        append_brake_action(&decision->trace,
                            BLHELI_S_BRAKE_ACTION_SET_DIRECTION_CHANGE_BRAKE);
        append_brake_action(&decision->trace,
                            BLHELI_S_BRAKE_ACTION_RESTORE_PWM_LIMIT_BEGIN);
        append_brake_action(&decision->trace,
                            BLHELI_S_BRAKE_ACTION_RETURN_TO_RUN4);
        return;
    }

    if (descriptor->commutation_period_four_x_high <
        decision->speed_threshold) {
        return;
    }

    if (!descriptor->direction_change_brake) {
        decision->result = BLHELI_S_BRAKE_STOP;
        decision->stop_due_to_speed = true;
        append_switch_power_off(decision);
        append_brake_action(&decision->trace,
                            BLHELI_S_BRAKE_ACTION_WAIT_FOR_PWM_OFF);
        append_switch_power_off(decision);
        decision->clears_runtime_flags = true;
        return;
    }

    decision->result = BLHELI_S_BRAKE_REVERSE_START;
    decision->direction_change_brake = false;
    decision->actual_reverse = descriptor->requested_reverse;
    decision->enters_initial_run = true;
    decision->initial_run_rotations = 18u;
    decision->brake_active = false;
    append_brake_action(&decision->trace,
                        BLHELI_S_BRAKE_ACTION_RESTORE_PWM_LIMIT_BEGIN);
    append_brake_action(&decision->trace,
                        BLHELI_S_BRAKE_ACTION_CLEAR_DIRECTION_CHANGE_BRAKE);
    append_brake_action(&decision->trace,
                        BLHELI_S_BRAKE_ACTION_SET_ACTUAL_DIRECTION);
    append_brake_action(&decision->trace,
                        BLHELI_S_BRAKE_ACTION_ENTER_INITIAL_RUN);
}

enum blheli_s_brake_result blheli_s_brake_update(
    struct blheli_s_brake_state *state, uint8_t stop_count,
    bool requested_reverse, bool actual_reverse, uint16_t speed_period)
{
    state->stop_count = stop_count;
    state->speed_period = speed_period;

    if (requested_reverse != actual_reverse) {
        state->direction_change = true;
        state->brake_active = true;
        return speed_period > 0x20u ? BLHELI_S_BRAKE_REVERSE_START
                                    : BLHELI_S_BRAKE_RUN;
    }
    if (stop_count >= (state->brake_on_stop ? 3u : 250u)) {
        state->brake_active = state->brake_on_stop;
        return BLHELI_S_BRAKE_STOP;
    }
    return BLHELI_S_BRAKE_RUN;
}
