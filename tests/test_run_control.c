#include <assert.h>

#include "core/bemf.h"
#include "core/commutation.h"
#include "core/run_control.h"

void test_run_control(void)
{
    struct blheli_s_run_state state;
    struct blheli_s_run_action_trace trace;
    struct blheli_s_commutation_action_trace commutation_trace;
    const struct blheli_s_run_step_descriptor *step;

    blheli_s_run_begin(&state, 160u);
    assert(state.step == BLHELI_S_COMMUTATION_STEP_1);
    assert(state.demag_metric == 120u);
    assert(blheli_s_run_update(&state, false, true, false) ==
           BLHELI_S_RUN_COMMUTATE);
    assert(state.step == BLHELI_S_COMMUTATION_STEP_2);
    assert(state.demag_metric == 120u);
    assert(blheli_s_run_update(&state, false, false, false) ==
           BLHELI_S_RUN_COMMUTATE);
    assert(state.demag_metric == 120u);
    assert(blheli_s_run_update(&state, true, false, false) ==
           BLHELI_S_RUN_SYNC_LOST);
    assert(blheli_s_run_update(&state, true, false, true) ==
           BLHELI_S_RUN_COMMUTATE);
    assert(blheli_s_run_update(&state, true, true, false) ==
           BLHELI_S_RUN_COMMUTATE);

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_1);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_2);
    assert(!step->start_adc_before_comparator);
    assert(!step->update_rpm_power_limit);
    assert(!step->check_temperature_power_after_commutation);
    assert(!step->run6_exit_checks);
    assert(blheli_s_run_build_action_trace(BLHELI_S_COMMUTATION_STEP_1,
                                           &trace));
    assert(trace.step == BLHELI_S_COMMUTATION_STEP_1);
    assert(trace.next_step == BLHELI_S_COMMUTATION_STEP_2);
    assert(trace.action_count == 6u);
    assert(trace.requires_zero_cross_timeout);
    assert(trace.sync_loss_possible);
    assert(!trace.updates_rpm_power_limit);
    assert(!trace.starts_adc_conversion);
    assert(trace.actions[0].kind ==
           BLHELI_S_RUN_ACTION_WAIT_BEFORE_ZERO_CROSS_SCAN);
    assert(trace.actions[1].kind ==
           BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_HIGH);
    assert(trace.actions[2].kind ==
           BLHELI_S_RUN_ACTION_EVALUATE_COMPARATOR_INTEGRITY);
    assert(trace.actions[3].kind ==
           BLHELI_S_RUN_ACTION_WAIT_FOR_COMMUTATION);
    assert(trace.actions[4].kind == BLHELI_S_RUN_ACTION_COMMUTATE);
    assert(trace.actions[4].from_step == BLHELI_S_COMMUTATION_STEP_1);
    assert(trace.actions[4].to_step == BLHELI_S_COMMUTATION_STEP_2);
    assert(trace.actions[5].kind ==
           BLHELI_S_RUN_ACTION_CALCULATE_NEXT_COMMUTATION_TIMING);
    assert(blheli_s_bemf_expected_high_for_run_transition(
               trace.comparator_transition, false));
    assert(blheli_s_commutation_build_action_trace(
        trace.step, BLHELI_S_COMMUTATION_FORWARD, &commutation_trace));
    assert(commutation_trace.from_step == trace.step);
    assert(commutation_trace.to_step == trace.next_step);

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_2);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_HIGH_TO_LOW);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_3);
    assert(step->update_rpm_power_limit);
    assert(blheli_s_run_build_action_trace(BLHELI_S_COMMUTATION_STEP_2,
                                           &trace));
    assert(trace.action_count == 7u);
    assert(trace.updates_rpm_power_limit);
    assert(trace.actions[1].kind ==
           BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_LOW);
    assert(trace.actions[3].kind ==
           BLHELI_S_RUN_ACTION_UPDATE_RPM_POWER_LIMIT);
    assert(trace.actions[4].kind ==
           BLHELI_S_RUN_ACTION_WAIT_FOR_COMMUTATION);
    assert(trace.actions[5].from_step == BLHELI_S_COMMUTATION_STEP_2);
    assert(trace.actions[5].to_step == BLHELI_S_COMMUTATION_STEP_3);
    assert(!blheli_s_bemf_expected_high_for_run_transition(
        trace.comparator_transition, false));

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_3);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_4);
    assert(blheli_s_run_build_action_trace(BLHELI_S_COMMUTATION_STEP_3,
                                           &trace));
    assert(trace.actions[1].kind ==
           BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_HIGH);
    assert(trace.actions[4].from_step == BLHELI_S_COMMUTATION_STEP_3);
    assert(trace.actions[4].to_step == BLHELI_S_COMMUTATION_STEP_4);

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_4);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_HIGH_TO_LOW);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_5);
    assert(blheli_s_run_build_action_trace(BLHELI_S_COMMUTATION_STEP_4,
                                           &trace));
    assert(trace.actions[1].kind ==
           BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_LOW);
    assert(trace.actions[4].from_step == BLHELI_S_COMMUTATION_STEP_4);
    assert(trace.actions[4].to_step == BLHELI_S_COMMUTATION_STEP_5);

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_5);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_6);
    assert(blheli_s_run_build_action_trace(BLHELI_S_COMMUTATION_STEP_5,
                                           &trace));
    assert(trace.actions[1].kind ==
           BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_HIGH);
    assert(trace.actions[4].from_step == BLHELI_S_COMMUTATION_STEP_5);
    assert(trace.actions[4].to_step == BLHELI_S_COMMUTATION_STEP_6);

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_6);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_HIGH_TO_LOW);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_1);
    assert(step->start_adc_before_comparator);
    assert(!step->update_rpm_power_limit);
    assert(step->check_temperature_power_after_commutation);
    assert(step->run6_exit_checks);
    assert(blheli_s_run_build_action_trace(BLHELI_S_COMMUTATION_STEP_6,
                                           &trace));
    assert(trace.action_count == 11u);
    assert(trace.starts_adc_conversion);
    assert(trace.checks_temperature_power);
    assert(trace.performs_run6_exit_checks);
    assert(trace.actions[0].kind ==
           BLHELI_S_RUN_ACTION_WAIT_BEFORE_ZERO_CROSS_SCAN);
    assert(trace.actions[1].kind == BLHELI_S_RUN_ACTION_START_ADC_CONVERSION);
    assert(trace.actions[2].kind ==
           BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_LOW);
    assert(trace.actions[5].kind == BLHELI_S_RUN_ACTION_COMMUTATE);
    assert(trace.actions[5].from_step == BLHELI_S_COMMUTATION_STEP_6);
    assert(trace.actions[5].to_step == BLHELI_S_COMMUTATION_STEP_1);
    assert(trace.actions[6].kind ==
           BLHELI_S_RUN_ACTION_CHECK_TEMPERATURE_POWER);
    assert(trace.actions[7].kind ==
           BLHELI_S_RUN_ACTION_CALCULATE_NEXT_COMMUTATION_TIMING);
    assert(trace.actions[8].kind ==
           BLHELI_S_RUN_ACTION_RUN6_STARTUP_CHECKS);
    assert(trace.actions[9].kind ==
           BLHELI_S_RUN_ACTION_RUN6_INITIAL_RUN_CHECKS);
    assert(trace.actions[10].kind ==
           BLHELI_S_RUN_ACTION_RUN6_STOP_TIMEOUT_DIRECTION_CHECKS);

    assert(blheli_s_run_step_descriptor(
               (enum blheli_s_commutation_step)0u) == 0);
    assert(!blheli_s_run_build_action_trace(
        (enum blheli_s_commutation_step)0u, &trace));
}
