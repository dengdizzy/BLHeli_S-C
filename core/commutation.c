#include "core/commutation.h"

static const struct blheli_s_commutation_action_trace forward_traces[] = {
    {
        .from_step = BLHELI_S_COMMUTATION_STEP_1,
        .to_step = BLHELI_S_COMMUTATION_STEP_2,
        .action_count = 7u,
        .requires_interrupt_mask = true,
        .actions = {
            { BLHELI_S_COMMUTATION_ACTION_SET_RPM_OUT,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_DISABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF,
              BLHELI_S_COMMUTATION_PHASE_B },
            { BLHELI_S_COMMUTATION_ACTION_COM_FET_ON,
              BLHELI_S_COMMUTATION_PHASE_A },
            { BLHELI_S_COMMUTATION_ACTION_SET_PWM_PHASE,
              BLHELI_S_COMMUTATION_PHASE_C },
            { BLHELI_S_COMMUTATION_ACTION_ENABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_SET_COMPARATOR_PHASE,
              BLHELI_S_COMMUTATION_PHASE_B }
        }
    },
    {
        .from_step = BLHELI_S_COMMUTATION_STEP_2,
        .to_step = BLHELI_S_COMMUTATION_STEP_3,
        .action_count = 7u,
        .requires_interrupt_mask = true,
        .actions = {
            { BLHELI_S_COMMUTATION_ACTION_CLEAR_RPM_OUT,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_DISABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_PWM_FET_OFF,
              BLHELI_S_COMMUTATION_PHASE_C },
            { BLHELI_S_COMMUTATION_ACTION_SET_PWM_PHASE,
              BLHELI_S_COMMUTATION_PHASE_B },
            { BLHELI_S_COMMUTATION_ACTION_COM_FET_ON,
              BLHELI_S_COMMUTATION_PHASE_A },
            { BLHELI_S_COMMUTATION_ACTION_ENABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_SET_COMPARATOR_PHASE,
              BLHELI_S_COMMUTATION_PHASE_C }
        }
    },
    {
        .from_step = BLHELI_S_COMMUTATION_STEP_3,
        .to_step = BLHELI_S_COMMUTATION_STEP_4,
        .action_count = 7u,
        .requires_interrupt_mask = true,
        .actions = {
            { BLHELI_S_COMMUTATION_ACTION_SET_RPM_OUT,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_DISABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF,
              BLHELI_S_COMMUTATION_PHASE_A },
            { BLHELI_S_COMMUTATION_ACTION_COM_FET_ON,
              BLHELI_S_COMMUTATION_PHASE_C },
            { BLHELI_S_COMMUTATION_ACTION_SET_PWM_PHASE,
              BLHELI_S_COMMUTATION_PHASE_B },
            { BLHELI_S_COMMUTATION_ACTION_ENABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_SET_COMPARATOR_PHASE,
              BLHELI_S_COMMUTATION_PHASE_A }
        }
    },
    {
        .from_step = BLHELI_S_COMMUTATION_STEP_4,
        .to_step = BLHELI_S_COMMUTATION_STEP_5,
        .action_count = 7u,
        .requires_interrupt_mask = true,
        .actions = {
            { BLHELI_S_COMMUTATION_ACTION_CLEAR_RPM_OUT,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_DISABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_PWM_FET_OFF,
              BLHELI_S_COMMUTATION_PHASE_B },
            { BLHELI_S_COMMUTATION_ACTION_SET_PWM_PHASE,
              BLHELI_S_COMMUTATION_PHASE_A },
            { BLHELI_S_COMMUTATION_ACTION_COM_FET_ON,
              BLHELI_S_COMMUTATION_PHASE_C },
            { BLHELI_S_COMMUTATION_ACTION_ENABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_SET_COMPARATOR_PHASE,
              BLHELI_S_COMMUTATION_PHASE_B }
        }
    },
    {
        .from_step = BLHELI_S_COMMUTATION_STEP_5,
        .to_step = BLHELI_S_COMMUTATION_STEP_6,
        .action_count = 7u,
        .requires_interrupt_mask = true,
        .actions = {
            { BLHELI_S_COMMUTATION_ACTION_SET_RPM_OUT,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_DISABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF,
              BLHELI_S_COMMUTATION_PHASE_C },
            { BLHELI_S_COMMUTATION_ACTION_COM_FET_ON,
              BLHELI_S_COMMUTATION_PHASE_B },
            { BLHELI_S_COMMUTATION_ACTION_SET_PWM_PHASE,
              BLHELI_S_COMMUTATION_PHASE_A },
            { BLHELI_S_COMMUTATION_ACTION_ENABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_SET_COMPARATOR_PHASE,
              BLHELI_S_COMMUTATION_PHASE_C }
        }
    },
    {
        .from_step = BLHELI_S_COMMUTATION_STEP_6,
        .to_step = BLHELI_S_COMMUTATION_STEP_1,
        .action_count = 7u,
        .requires_interrupt_mask = true,
        .actions = {
            { BLHELI_S_COMMUTATION_ACTION_CLEAR_RPM_OUT,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_DISABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_PWM_FET_OFF,
              BLHELI_S_COMMUTATION_PHASE_A },
            { BLHELI_S_COMMUTATION_ACTION_SET_PWM_PHASE,
              BLHELI_S_COMMUTATION_PHASE_C },
            { BLHELI_S_COMMUTATION_ACTION_COM_FET_ON,
              BLHELI_S_COMMUTATION_PHASE_B },
            { BLHELI_S_COMMUTATION_ACTION_ENABLE_INTERRUPTS,
              BLHELI_S_COMMUTATION_PHASE_NONE },
            { BLHELI_S_COMMUTATION_ACTION_SET_COMPARATOR_PHASE,
              BLHELI_S_COMMUTATION_PHASE_A }
        }
    }
};

enum blheli_s_commutation_step blheli_s_commutation_next_step(
    enum blheli_s_commutation_step step)
{
    if (step == BLHELI_S_COMMUTATION_STEP_6) {
        return BLHELI_S_COMMUTATION_STEP_1;
    }
    if (blheli_s_commutation_step_is_valid(step)) {
        return (enum blheli_s_commutation_step)((uint8_t)step + 1u);
    }
    return BLHELI_S_COMMUTATION_STEP_1;
}

bool blheli_s_commutation_step_is_valid(
    enum blheli_s_commutation_step step)
{
    return step >= BLHELI_S_COMMUTATION_STEP_1 &&
           step <= BLHELI_S_COMMUTATION_STEP_6;
}

bool blheli_s_commutation_rpm_output_enabled(
    enum blheli_s_commutation_step step)
{
    return step == BLHELI_S_COMMUTATION_STEP_2 ||
           step == BLHELI_S_COMMUTATION_STEP_4 ||
           step == BLHELI_S_COMMUTATION_STEP_6;
}

bool blheli_s_commutation_build_action_trace(
    enum blheli_s_commutation_step step,
    enum blheli_s_commutation_direction direction,
    struct blheli_s_commutation_action_trace *trace)
{
    if (trace == 0) {
        return false;
    }
    *trace = (struct blheli_s_commutation_action_trace){0};
    if (!blheli_s_commutation_step_is_valid(step) ||
        direction != BLHELI_S_COMMUTATION_FORWARD) {
        return false;
    }
    *trace = forward_traces[(uint8_t)step - 1u];
    return true;
}
