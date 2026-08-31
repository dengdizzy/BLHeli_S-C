#ifndef BLHELI_S_BRAKE_H
#define BLHELI_S_BRAKE_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_brake_result {
    BLHELI_S_BRAKE_RUN,
    BLHELI_S_BRAKE_STOP,
    BLHELI_S_BRAKE_REVERSE_START
};

#define BLHELI_S_BRAKE_MAX_ACTIONS 12u

enum blheli_s_brake_action_kind {
    BLHELI_S_BRAKE_ACTION_RESET_STALL_COUNT,
    BLHELI_S_BRAKE_ACTION_SET_DIRECTION_CHANGE_BRAKE,
    BLHELI_S_BRAKE_ACTION_RESTORE_PWM_LIMIT_BEGIN,
    BLHELI_S_BRAKE_ACTION_RETURN_TO_RUN4,
    BLHELI_S_BRAKE_ACTION_CLEAR_DIRECTION_CHANGE_BRAKE,
    BLHELI_S_BRAKE_ACTION_SET_ACTUAL_DIRECTION,
    BLHELI_S_BRAKE_ACTION_ENTER_INITIAL_RUN,
    BLHELI_S_BRAKE_ACTION_SWITCH_POWER_OFF_DISABLE_INTERRUPTS,
    BLHELI_S_BRAKE_ACTION_ALL_PWM_FETS_OFF,
    BLHELI_S_BRAKE_ACTION_ALL_COMMUTATION_FETS_OFF,
    BLHELI_S_BRAKE_ACTION_SET_PWMS_OFF,
    BLHELI_S_BRAKE_ACTION_WAIT_FOR_PWM_OFF,
    BLHELI_S_BRAKE_ACTION_ALL_COMMUTATION_FETS_ON
};

struct blheli_s_brake_action_trace {
    uint8_t action_count;
    enum blheli_s_brake_action_kind actions[BLHELI_S_BRAKE_MAX_ACTIONS];
};

struct blheli_s_brake_state {
    bool direction_change;
    bool brake_on_stop;
    bool brake_active;
    uint8_t stop_count;
    uint16_t speed_period;
};

struct blheli_s_brake_descriptor {
    bool bidirectional;
    bool brake_on_stop;
    bool requested_reverse;
    bool actual_reverse;
    bool direction_change_brake;
    uint8_t stop_count;
    uint8_t rcp_timeout_countdown;
    uint8_t commutation_period_four_x_high;
    uint8_t pwm_limit_begin;
};

struct blheli_s_brake_decision {
    enum blheli_s_brake_result result;
    struct blheli_s_brake_action_trace trace;
    uint8_t stop_threshold;
    uint8_t speed_threshold;
    uint8_t pwm_limit;
    uint8_t initial_run_rotations;
    bool direction_change_brake;
    bool actual_reverse;
    bool brake_active;
    bool stop_due_to_stop_count;
    bool stop_due_to_timeout;
    bool stop_due_to_speed;
    bool returns_to_run4;
    bool enters_initial_run;
    bool clears_runtime_flags;
    bool switch_power_off;
    bool brake_on_stop_fets_on;
};

void blheli_s_brake_begin(struct blheli_s_brake_state *state,
                          bool brake_on_stop);
enum blheli_s_brake_result blheli_s_brake_update(
    struct blheli_s_brake_state *state, uint8_t stop_count,
    bool requested_reverse, bool actual_reverse, uint16_t speed_period);
void blheli_s_brake_evaluate(const struct blheli_s_brake_descriptor *descriptor,
                             struct blheli_s_brake_decision *decision);

#endif
