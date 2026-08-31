#ifndef BLHELI_S_BEMF_H
#define BLHELI_S_BEMF_H

#include <stdbool.h>
#include <stdint.h>

#include "core/run_control.h"

enum blheli_s_bemf_result {
    BLHELI_S_BEMF_WAITING,
    BLHELI_S_BEMF_ACCEPTED,
    BLHELI_S_BEMF_REJECTED,
    BLHELI_S_BEMF_TIMED_OUT
};

enum blheli_s_bemf_wait {
    BLHELI_S_BEMF_WAIT_LOW,
    BLHELI_S_BEMF_WAIT_HIGH
};

struct blheli_s_bemf_wait_descriptor {
    enum blheli_s_bemf_wait wait;
    uint8_t required_stable_samples;
    uint8_t maximum_required_samples;
    bool expected_high;
    bool initial_demag_detected;
};

struct blheli_s_bemf_sample_state {
    uint8_t reads;
    uint8_t stable_reads;
    uint8_t required;
    uint8_t maximum_required;
    bool expected_high;
    bool demag_detected;
    bool timed_out;
};

void blheli_s_bemf_describe_wait(
    struct blheli_s_bemf_wait_descriptor *descriptor,
    enum blheli_s_bemf_wait wait, bool direction_change_brake,
    bool startup_phase, bool initial_run_phase, bool high_rpm,
    uint8_t commutation_period_four_x_high, bool clock_48mhz);
bool blheli_s_bemf_expected_high_for_run_transition(
    enum blheli_s_run_comparator_transition transition,
    bool direction_change_brake);
void blheli_s_bemf_begin(struct blheli_s_bemf_sample_state *state,
                         bool expected_high, uint8_t required);
void blheli_s_bemf_begin_from_descriptor(
    struct blheli_s_bemf_sample_state *state,
    const struct blheli_s_bemf_wait_descriptor *descriptor);
enum blheli_s_bemf_result blheli_s_bemf_sample(
    struct blheli_s_bemf_sample_state *state, bool comparator_high);
enum blheli_s_bemf_result blheli_s_bemf_sample_with_timeout(
    struct blheli_s_bemf_sample_state *state, bool comparator_high,
    bool timed_out);

#endif
