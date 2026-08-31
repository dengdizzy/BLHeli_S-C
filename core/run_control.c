#include "core/run_control.h"

static const struct blheli_s_run_step_descriptor run_steps[] = {
    {
        .step = BLHELI_S_COMMUTATION_STEP_1,
        .power_on_phase = BLHELI_S_RUN_PHASE_B,
        .pwm_phase = BLHELI_S_RUN_PHASE_C,
        .comparator_phase = BLHELI_S_RUN_PHASE_A,
        .comparator_transition = BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH,
        .next_step = BLHELI_S_COMMUTATION_STEP_2,
        .start_adc_before_comparator = false,
        .update_rpm_power_limit = false,
        .check_temperature_power_after_commutation = false,
        .run6_exit_checks = false
    },
    {
        .step = BLHELI_S_COMMUTATION_STEP_2,
        .power_on_phase = BLHELI_S_RUN_PHASE_A,
        .pwm_phase = BLHELI_S_RUN_PHASE_C,
        .comparator_phase = BLHELI_S_RUN_PHASE_B,
        .comparator_transition = BLHELI_S_RUN_COMPARATOR_HIGH_TO_LOW,
        .next_step = BLHELI_S_COMMUTATION_STEP_3,
        .start_adc_before_comparator = false,
        .update_rpm_power_limit = true,
        .check_temperature_power_after_commutation = false,
        .run6_exit_checks = false
    },
    {
        .step = BLHELI_S_COMMUTATION_STEP_3,
        .power_on_phase = BLHELI_S_RUN_PHASE_A,
        .pwm_phase = BLHELI_S_RUN_PHASE_B,
        .comparator_phase = BLHELI_S_RUN_PHASE_C,
        .comparator_transition = BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH,
        .next_step = BLHELI_S_COMMUTATION_STEP_4,
        .start_adc_before_comparator = false,
        .update_rpm_power_limit = false,
        .check_temperature_power_after_commutation = false,
        .run6_exit_checks = false
    },
    {
        .step = BLHELI_S_COMMUTATION_STEP_4,
        .power_on_phase = BLHELI_S_RUN_PHASE_C,
        .pwm_phase = BLHELI_S_RUN_PHASE_B,
        .comparator_phase = BLHELI_S_RUN_PHASE_A,
        .comparator_transition = BLHELI_S_RUN_COMPARATOR_HIGH_TO_LOW,
        .next_step = BLHELI_S_COMMUTATION_STEP_5,
        .start_adc_before_comparator = false,
        .update_rpm_power_limit = false,
        .check_temperature_power_after_commutation = false,
        .run6_exit_checks = false
    },
    {
        .step = BLHELI_S_COMMUTATION_STEP_5,
        .power_on_phase = BLHELI_S_RUN_PHASE_C,
        .pwm_phase = BLHELI_S_RUN_PHASE_A,
        .comparator_phase = BLHELI_S_RUN_PHASE_B,
        .comparator_transition = BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH,
        .next_step = BLHELI_S_COMMUTATION_STEP_6,
        .start_adc_before_comparator = false,
        .update_rpm_power_limit = false,
        .check_temperature_power_after_commutation = false,
        .run6_exit_checks = false
    },
    {
        .step = BLHELI_S_COMMUTATION_STEP_6,
        .power_on_phase = BLHELI_S_RUN_PHASE_B,
        .pwm_phase = BLHELI_S_RUN_PHASE_A,
        .comparator_phase = BLHELI_S_RUN_PHASE_C,
        .comparator_transition = BLHELI_S_RUN_COMPARATOR_HIGH_TO_LOW,
        .next_step = BLHELI_S_COMMUTATION_STEP_1,
        .start_adc_before_comparator = true,
        .update_rpm_power_limit = false,
        .check_temperature_power_after_commutation = true,
        .run6_exit_checks = true
    }
};

const struct blheli_s_run_step_descriptor *blheli_s_run_step_descriptor(
    enum blheli_s_commutation_step step)
{
    if (!blheli_s_commutation_step_is_valid(step)) {
        return 0;
    }
    return &run_steps[(uint8_t)step - 1u];
}

static void append_run_action(struct blheli_s_run_action_trace *trace,
                              enum blheli_s_run_action_kind kind)
{
    trace->actions[trace->action_count++] =
        (struct blheli_s_run_action){ .kind = kind };
}

bool blheli_s_run_build_action_trace(enum blheli_s_commutation_step step,
                                     struct blheli_s_run_action_trace *trace)
{
    const struct blheli_s_run_step_descriptor *descriptor =
        blheli_s_run_step_descriptor(step);

    if (descriptor == 0) {
        return false;
    }

    *trace = (struct blheli_s_run_action_trace){
        .step = descriptor->step,
        .next_step = descriptor->next_step,
        .comparator_transition = descriptor->comparator_transition,
        .requires_zero_cross_timeout = true,
        .sync_loss_possible = true,
        .updates_rpm_power_limit = descriptor->update_rpm_power_limit,
        .starts_adc_conversion = descriptor->start_adc_before_comparator,
        .checks_temperature_power =
            descriptor->check_temperature_power_after_commutation,
        .performs_run6_exit_checks = descriptor->run6_exit_checks
    };

    append_run_action(trace,
                      BLHELI_S_RUN_ACTION_WAIT_BEFORE_ZERO_CROSS_SCAN);
    if (descriptor->start_adc_before_comparator) {
        append_run_action(trace, BLHELI_S_RUN_ACTION_START_ADC_CONVERSION);
    }
    append_run_action(
        trace,
        descriptor->comparator_transition ==
                BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH
            ? BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_HIGH
            : BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_LOW);
    append_run_action(trace,
                      BLHELI_S_RUN_ACTION_EVALUATE_COMPARATOR_INTEGRITY);
    if (descriptor->update_rpm_power_limit) {
        append_run_action(trace, BLHELI_S_RUN_ACTION_UPDATE_RPM_POWER_LIMIT);
    }
    append_run_action(trace, BLHELI_S_RUN_ACTION_WAIT_FOR_COMMUTATION);
    trace->actions[trace->action_count++] = (struct blheli_s_run_action){
        .kind = BLHELI_S_RUN_ACTION_COMMUTATE,
        .from_step = descriptor->step,
        .to_step = descriptor->next_step
    };
    if (descriptor->check_temperature_power_after_commutation) {
        append_run_action(trace, BLHELI_S_RUN_ACTION_CHECK_TEMPERATURE_POWER);
    }
    append_run_action(trace,
                      BLHELI_S_RUN_ACTION_CALCULATE_NEXT_COMMUTATION_TIMING);
    if (descriptor->run6_exit_checks) {
        append_run_action(trace, BLHELI_S_RUN_ACTION_RUN6_STARTUP_CHECKS);
        append_run_action(trace, BLHELI_S_RUN_ACTION_RUN6_INITIAL_RUN_CHECKS);
        append_run_action(
            trace, BLHELI_S_RUN_ACTION_RUN6_STOP_TIMEOUT_DIRECTION_CHECKS);
    }

    return true;
}

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
    state->step = blheli_s_run_step_descriptor(state->step)->next_step;
    return BLHELI_S_RUN_COMMUTATE;
}
