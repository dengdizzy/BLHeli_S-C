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
void blheli_s_run_begin(struct blheli_s_run_state *state,
                        uint8_t demag_power_off_threshold);
enum blheli_s_run_result blheli_s_run_update(
    struct blheli_s_run_state *state, bool zero_cross_timeout,
    bool demag_detected, bool direction_change_brake);

#endif
