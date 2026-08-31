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
