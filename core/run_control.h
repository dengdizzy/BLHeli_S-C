#ifndef BLHELI_S_RUN_CONTROL_H
#define BLHELI_S_RUN_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

#include "core/commutation.h"

enum blheli_s_run_result {
    BLHELI_S_RUN_WAITING,
    BLHELI_S_RUN_COMMUTATE,
    BLHELI_S_RUN_SYNC_LOST
};

enum blheli_s_run_phase {
    BLHELI_S_RUN_PHASE_A,
    BLHELI_S_RUN_PHASE_B,
    BLHELI_S_RUN_PHASE_C
};

enum blheli_s_run_comparator_transition {
    BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH,
    BLHELI_S_RUN_COMPARATOR_HIGH_TO_LOW
};

#define BLHELI_S_RUN_MAX_ACTIONS 12u

enum blheli_s_run_action_kind {
    BLHELI_S_RUN_ACTION_WAIT_BEFORE_ZERO_CROSS_SCAN,
    BLHELI_S_RUN_ACTION_START_ADC_CONVERSION,
    BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_LOW,
    BLHELI_S_RUN_ACTION_WAIT_FOR_COMPARATOR_HIGH,
    BLHELI_S_RUN_ACTION_EVALUATE_COMPARATOR_INTEGRITY,
    BLHELI_S_RUN_ACTION_UPDATE_RPM_POWER_LIMIT,
    BLHELI_S_RUN_ACTION_WAIT_FOR_COMMUTATION,
    BLHELI_S_RUN_ACTION_COMMUTATE,
    BLHELI_S_RUN_ACTION_CHECK_TEMPERATURE_POWER,
    BLHELI_S_RUN_ACTION_CALCULATE_NEXT_COMMUTATION_TIMING,
    BLHELI_S_RUN_ACTION_RUN6_STARTUP_CHECKS,
    BLHELI_S_RUN_ACTION_RUN6_INITIAL_RUN_CHECKS,
    BLHELI_S_RUN_ACTION_RUN6_STOP_TIMEOUT_DIRECTION_CHECKS
};

struct blheli_s_run_action {
    enum blheli_s_run_action_kind kind;
    enum blheli_s_commutation_step from_step;
    enum blheli_s_commutation_step to_step;
};

struct blheli_s_run_action_trace {
    enum blheli_s_commutation_step step;
    enum blheli_s_commutation_step next_step;
    enum blheli_s_run_comparator_transition comparator_transition;
    uint8_t action_count;
    bool requires_zero_cross_timeout;
    bool sync_loss_possible;
    bool updates_rpm_power_limit;
    bool starts_adc_conversion;
    bool checks_temperature_power;
    bool performs_run6_exit_checks;
    struct blheli_s_run_action actions[BLHELI_S_RUN_MAX_ACTIONS];
};

struct blheli_s_run_step_descriptor {
    enum blheli_s_commutation_step step;
    enum blheli_s_run_phase power_on_phase;
    enum blheli_s_run_phase pwm_phase;
    enum blheli_s_run_phase comparator_phase;
    enum blheli_s_run_comparator_transition comparator_transition;
    enum blheli_s_commutation_step next_step;
    bool start_adc_before_comparator;
    bool update_rpm_power_limit;
    bool check_temperature_power_after_commutation;
    bool run6_exit_checks;
};

struct blheli_s_run_state {
    enum blheli_s_commutation_step step;
    uint8_t demag_metric;
    uint8_t demag_power_off_threshold;
};

const struct blheli_s_run_step_descriptor *blheli_s_run_step_descriptor(
    enum blheli_s_commutation_step step);
bool blheli_s_run_build_action_trace(enum blheli_s_commutation_step step,
                                     struct blheli_s_run_action_trace *trace);
void blheli_s_run_begin(struct blheli_s_run_state *state,
                        uint8_t demag_power_off_threshold);
enum blheli_s_run_result blheli_s_run_update(
    struct blheli_s_run_state *state, bool zero_cross_timeout,
    bool demag_detected, bool direction_change_brake);

#endif
