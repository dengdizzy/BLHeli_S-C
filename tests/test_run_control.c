#include <assert.h>

#include "core/run_control.h"

void test_run_control(void)
{
    struct blheli_s_run_state state;
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

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_2);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_HIGH_TO_LOW);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_3);
    assert(step->update_rpm_power_limit);

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_3);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_4);

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_4);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_HIGH_TO_LOW);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_5);

    step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_5);
    assert(step != 0);
    assert(step->power_on_phase == BLHELI_S_RUN_PHASE_C);
    assert(step->pwm_phase == BLHELI_S_RUN_PHASE_A);
    assert(step->comparator_phase == BLHELI_S_RUN_PHASE_B);
    assert(step->comparator_transition ==
           BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH);
    assert(step->next_step == BLHELI_S_COMMUTATION_STEP_6);

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

    assert(blheli_s_run_step_descriptor(
               (enum blheli_s_commutation_step)0u) == 0);
}
