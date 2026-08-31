#ifndef BLHELI_S_STARTUP_H
#define BLHELI_S_STARTUP_H

#include <stdbool.h>
#include <stdint.h>

#include "core/commutation.h"

enum blheli_s_startup_result {
    BLHELI_S_STARTUP_CONTINUE,
    BLHELI_S_STARTUP_INITIAL_RUN,
    BLHELI_S_STARTUP_STOP
};

#define BLHELI_S_STARTUP_MAX_ACTIONS 20u
#define BLHELI_S_STARTUP_INITIAL_COMMUTATIONS 2u

enum blheli_s_startup_action_kind {
    BLHELI_S_STARTUP_ACTION_DISABLE_INTERRUPTS,
    BLHELI_S_STARTUP_ACTION_SWITCH_POWER_OFF,
    BLHELI_S_STARTUP_ACTION_ENABLE_INTERRUPTS,
    BLHELI_S_STARTUP_ACTION_CLEAR_RUNTIME_FLAGS,
    BLHELI_S_STARTUP_ACTION_REQUEST_INITIAL_TEMPERATURE,
    BLHELI_S_STARTUP_ACTION_CHECK_TEMPERATURE_POWER,
    BLHELI_S_STARTUP_ACTION_CALCULATE_STARTUP_PWM,
    BLHELI_S_STARTUP_ACTION_RESTORE_PWM_LIMITS,
    BLHELI_S_STARTUP_ACTION_SELECT_DIRECTION,
    BLHELI_S_STARTUP_ACTION_ENTER_STARTUP_PHASE,
    BLHELI_S_STARTUP_ACTION_COMMUTATE,
    BLHELI_S_STARTUP_ACTION_INITIALIZE_TIMING,
    BLHELI_S_STARTUP_ACTION_CALCULATE_VIRTUAL_COMMUTATION
};

struct blheli_s_startup_action {
    enum blheli_s_startup_action_kind kind;
    enum blheli_s_commutation_step from_step;
    enum blheli_s_commutation_step to_step;
};

struct blheli_s_startup_action_trace {
    uint8_t action_count;
    bool direction_reversed;
    bool requests_initial_temperature;
    bool enters_startup_phase;
    uint8_t timing_initialization_count;
    uint8_t virtual_commutation_calculation_count;
    struct blheli_s_startup_action actions[BLHELI_S_STARTUP_MAX_ACTIONS];
};

struct blheli_s_startup_state {
    uint8_t commutation_count;
    uint8_t initial_run_rotations_remaining;
    uint8_t pwm_limit_begin;
    uint8_t pwm_limit;
    uint8_t pwm_limit_by_rpm;
    uint16_t initial_commutation_period_four_x;
    uint8_t initial_commutation_count;
    enum blheli_s_commutation_step initial_commutation_from[
        BLHELI_S_STARTUP_INITIAL_COMMUTATIONS];
    enum blheli_s_commutation_step initial_commutation_to[
        BLHELI_S_STARTUP_INITIAL_COMMUTATIONS];
    uint8_t timing_initialization_count;
    uint8_t virtual_commutation_calculation_count;
    bool initial_temperature_requested;
    bool direction_reversed;
};

uint8_t blheli_s_startup_pwm_limit(uint8_t startup_power_decoded);
uint16_t blheli_s_startup_initial_commutation_period_four_x(void);
void blheli_s_startup_build_action_trace(
    struct blheli_s_startup_action_trace *trace, bool direction_reversed);
void blheli_s_startup_begin(struct blheli_s_startup_state *state,
                            uint8_t startup_pwm_limit,
                            bool direction_reversed);
enum blheli_s_startup_result blheli_s_startup_after_commutation(
    struct blheli_s_startup_state *state, uint8_t throttle);
enum blheli_s_startup_result blheli_s_startup_after_rotation(
    struct blheli_s_startup_state *state);

#endif
