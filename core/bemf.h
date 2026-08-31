#ifndef BLHELI_S_BEMF_H
#define BLHELI_S_BEMF_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_bemf_result {
    BLHELI_S_BEMF_WAITING,
    BLHELI_S_BEMF_ACCEPTED,
    BLHELI_S_BEMF_REJECTED,
    BLHELI_S_BEMF_TIMED_OUT
};

struct blheli_s_bemf_sample_state {
    uint8_t reads;
    uint8_t required;
    bool expected_high;
    bool demag_detected;
    bool timed_out;
};

void blheli_s_bemf_begin(struct blheli_s_bemf_sample_state *state,
                         bool expected_high, uint8_t required);
enum blheli_s_bemf_result blheli_s_bemf_sample(
    struct blheli_s_bemf_sample_state *state, bool comparator_high);

#endif
