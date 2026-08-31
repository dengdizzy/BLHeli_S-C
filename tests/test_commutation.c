#include <assert.h>

#include "core/commutation.h"
#include "hal/phase_mapping.h"

static void assert_action(
    const struct blheli_s_commutation_action_trace *trace, uint8_t index,
    enum blheli_s_commutation_action_kind kind,
    enum blheli_s_commutation_phase phase)
{
    assert(trace->actions[index].kind == kind);
    assert(trace->actions[index].phase == phase);
}

static void assert_forward_trace(
    enum blheli_s_commutation_step from_step,
    enum blheli_s_commutation_step to_step,
    enum blheli_s_commutation_action_kind rpm_action,
    enum blheli_s_commutation_action_kind off_action,
    enum blheli_s_commutation_phase off_phase,
    enum blheli_s_commutation_phase on_phase,
    enum blheli_s_commutation_phase pwm_phase,
    enum blheli_s_commutation_phase comparator_phase)
{
    struct blheli_s_commutation_action_trace trace;

    assert(blheli_s_commutation_build_action_trace(
        from_step, BLHELI_S_COMMUTATION_FORWARD, &trace));
    assert(trace.from_step == from_step);
    assert(trace.to_step == to_step);
    assert(trace.action_count == 7u);
    assert(trace.requires_interrupt_mask);
    assert_action(&trace, 0u, rpm_action, BLHELI_S_COMMUTATION_PHASE_NONE);
    assert_action(&trace, 1u, BLHELI_S_COMMUTATION_ACTION_DISABLE_INTERRUPTS,
                  BLHELI_S_COMMUTATION_PHASE_NONE);
    assert_action(&trace, 2u, off_action, off_phase);
    assert_action(&trace, 3u,
                  off_action == BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF
                      ? BLHELI_S_COMMUTATION_ACTION_COM_FET_ON
                      : BLHELI_S_COMMUTATION_ACTION_SET_PWM_PHASE,
                  off_action == BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF
                      ? on_phase
                      : pwm_phase);
    assert_action(&trace, 4u,
                  off_action == BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF
                      ? BLHELI_S_COMMUTATION_ACTION_SET_PWM_PHASE
                      : BLHELI_S_COMMUTATION_ACTION_COM_FET_ON,
                  off_action == BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF
                      ? pwm_phase
                      : on_phase);
    assert_action(&trace, 5u, BLHELI_S_COMMUTATION_ACTION_ENABLE_INTERRUPTS,
                  BLHELI_S_COMMUTATION_PHASE_NONE);
    assert_action(&trace, 6u,
                  BLHELI_S_COMMUTATION_ACTION_SET_COMPARATOR_PHASE,
                  comparator_phase);
}

void test_commutation(void)
{
    enum blheli_s_commutation_step step = BLHELI_S_COMMUTATION_STEP_1;
    const struct esc_hal_phase_mapping *mapping;
    struct blheli_s_commutation_action_trace trace;

    for (unsigned int count = 0u; count < 6u; ++count) {
        assert(blheli_s_commutation_step_is_valid(step));
        step = blheli_s_commutation_next_step(step);
    }
    assert(step == BLHELI_S_COMMUTATION_STEP_1);
    assert(blheli_s_commutation_next_step(0) ==
           BLHELI_S_COMMUTATION_STEP_1);
    assert(blheli_s_commutation_rpm_output_enabled(
               BLHELI_S_COMMUTATION_STEP_2));
    assert(!blheli_s_commutation_rpm_output_enabled(
               BLHELI_S_COMMUTATION_STEP_1));

    mapping = esc_hal_get_phase_mapping(
        BLHELI_S_COMMUTATION_STEP_1, BLHELI_S_COMMUTATION_FORWARD);
    assert(mapping->pwm_phase == ESC_HAL_PHASE_B);
    assert(mapping->commutation_phase == ESC_HAL_PHASE_C);
    assert(mapping->bemf_phase == ESC_HAL_PHASE_A);

    mapping = esc_hal_get_phase_mapping(
        BLHELI_S_COMMUTATION_STEP_1, BLHELI_S_COMMUTATION_REVERSE);
    assert(mapping->pwm_phase == ESC_HAL_PHASE_A);
    assert(mapping->commutation_phase == ESC_HAL_PHASE_B);
    assert(mapping->bemf_phase == ESC_HAL_PHASE_C);
    assert(esc_hal_get_phase_mapping(0,
                                     BLHELI_S_COMMUTATION_FORWARD) == 0);

    assert_forward_trace(
        BLHELI_S_COMMUTATION_STEP_1, BLHELI_S_COMMUTATION_STEP_2,
        BLHELI_S_COMMUTATION_ACTION_SET_RPM_OUT,
        BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF,
        BLHELI_S_COMMUTATION_PHASE_B, BLHELI_S_COMMUTATION_PHASE_A,
        BLHELI_S_COMMUTATION_PHASE_C, BLHELI_S_COMMUTATION_PHASE_B);
    assert_forward_trace(
        BLHELI_S_COMMUTATION_STEP_2, BLHELI_S_COMMUTATION_STEP_3,
        BLHELI_S_COMMUTATION_ACTION_CLEAR_RPM_OUT,
        BLHELI_S_COMMUTATION_ACTION_PWM_FET_OFF,
        BLHELI_S_COMMUTATION_PHASE_C, BLHELI_S_COMMUTATION_PHASE_A,
        BLHELI_S_COMMUTATION_PHASE_B, BLHELI_S_COMMUTATION_PHASE_C);
    assert_forward_trace(
        BLHELI_S_COMMUTATION_STEP_3, BLHELI_S_COMMUTATION_STEP_4,
        BLHELI_S_COMMUTATION_ACTION_SET_RPM_OUT,
        BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF,
        BLHELI_S_COMMUTATION_PHASE_A, BLHELI_S_COMMUTATION_PHASE_C,
        BLHELI_S_COMMUTATION_PHASE_B, BLHELI_S_COMMUTATION_PHASE_A);
    assert_forward_trace(
        BLHELI_S_COMMUTATION_STEP_4, BLHELI_S_COMMUTATION_STEP_5,
        BLHELI_S_COMMUTATION_ACTION_CLEAR_RPM_OUT,
        BLHELI_S_COMMUTATION_ACTION_PWM_FET_OFF,
        BLHELI_S_COMMUTATION_PHASE_B, BLHELI_S_COMMUTATION_PHASE_C,
        BLHELI_S_COMMUTATION_PHASE_A, BLHELI_S_COMMUTATION_PHASE_B);
    assert_forward_trace(
        BLHELI_S_COMMUTATION_STEP_5, BLHELI_S_COMMUTATION_STEP_6,
        BLHELI_S_COMMUTATION_ACTION_SET_RPM_OUT,
        BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF,
        BLHELI_S_COMMUTATION_PHASE_C, BLHELI_S_COMMUTATION_PHASE_B,
        BLHELI_S_COMMUTATION_PHASE_A, BLHELI_S_COMMUTATION_PHASE_C);
    assert_forward_trace(
        BLHELI_S_COMMUTATION_STEP_6, BLHELI_S_COMMUTATION_STEP_1,
        BLHELI_S_COMMUTATION_ACTION_CLEAR_RPM_OUT,
        BLHELI_S_COMMUTATION_ACTION_PWM_FET_OFF,
        BLHELI_S_COMMUTATION_PHASE_A, BLHELI_S_COMMUTATION_PHASE_B,
        BLHELI_S_COMMUTATION_PHASE_C, BLHELI_S_COMMUTATION_PHASE_A);

    assert(!blheli_s_commutation_build_action_trace(
        (enum blheli_s_commutation_step)0u, BLHELI_S_COMMUTATION_FORWARD,
        &trace));
    assert(trace.action_count == 0u);
    assert(!blheli_s_commutation_build_action_trace(
        BLHELI_S_COMMUTATION_STEP_1, BLHELI_S_COMMUTATION_REVERSE,
        &trace));
    assert(trace.action_count == 0u);
    assert(!blheli_s_commutation_build_action_trace(
        BLHELI_S_COMMUTATION_STEP_1, BLHELI_S_COMMUTATION_FORWARD, 0));
}
