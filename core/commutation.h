#ifndef BLHELI_S_COMMUTATION_H
#define BLHELI_S_COMMUTATION_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_commutation_step {
    BLHELI_S_COMMUTATION_STEP_1 = 1u,
    BLHELI_S_COMMUTATION_STEP_2,
    BLHELI_S_COMMUTATION_STEP_3,
    BLHELI_S_COMMUTATION_STEP_4,
    BLHELI_S_COMMUTATION_STEP_5,
    BLHELI_S_COMMUTATION_STEP_6
};

enum blheli_s_commutation_direction {
    BLHELI_S_COMMUTATION_FORWARD,
    BLHELI_S_COMMUTATION_REVERSE
};

#define BLHELI_S_COMMUTATION_MAX_ACTIONS 8u

enum blheli_s_commutation_phase {
    BLHELI_S_COMMUTATION_PHASE_NONE,
    BLHELI_S_COMMUTATION_PHASE_A,
    BLHELI_S_COMMUTATION_PHASE_B,
    BLHELI_S_COMMUTATION_PHASE_C
};

enum blheli_s_commutation_action_kind {
    BLHELI_S_COMMUTATION_ACTION_SET_RPM_OUT,
    BLHELI_S_COMMUTATION_ACTION_CLEAR_RPM_OUT,
    BLHELI_S_COMMUTATION_ACTION_DISABLE_INTERRUPTS,
    BLHELI_S_COMMUTATION_ACTION_ENABLE_INTERRUPTS,
    BLHELI_S_COMMUTATION_ACTION_COM_FET_OFF,
    BLHELI_S_COMMUTATION_ACTION_COM_FET_ON,
    BLHELI_S_COMMUTATION_ACTION_PWM_FET_OFF,
    BLHELI_S_COMMUTATION_ACTION_SET_PWM_PHASE,
    BLHELI_S_COMMUTATION_ACTION_SET_COMPARATOR_PHASE
};

struct blheli_s_commutation_action {
    enum blheli_s_commutation_action_kind kind;
    enum blheli_s_commutation_phase phase;
};

struct blheli_s_commutation_action_trace {
    enum blheli_s_commutation_step from_step;
    enum blheli_s_commutation_step to_step;
    uint8_t action_count;
    bool requires_interrupt_mask;
    struct blheli_s_commutation_action
        actions[BLHELI_S_COMMUTATION_MAX_ACTIONS];
};

enum blheli_s_commutation_step blheli_s_commutation_next_step(
    enum blheli_s_commutation_step step);
bool blheli_s_commutation_step_is_valid(
    enum blheli_s_commutation_step step);
bool blheli_s_commutation_rpm_output_enabled(
    enum blheli_s_commutation_step step);
bool blheli_s_commutation_build_action_trace(
    enum blheli_s_commutation_step step,
    enum blheli_s_commutation_direction direction,
    struct blheli_s_commutation_action_trace *trace);

#endif
