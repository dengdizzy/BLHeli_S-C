#include "core/bemf.h"

void blheli_s_bemf_begin(struct blheli_s_bemf_sample_state *state,
                         bool expected_high, uint8_t required)
{
    *state = (struct blheli_s_bemf_sample_state){
        .reads = 0u,
        .required = required == 0u ? 1u : required,
        .expected_high = expected_high,
        .demag_detected = true,
        .timed_out = false
    };
}

enum blheli_s_bemf_result blheli_s_bemf_sample(
    struct blheli_s_bemf_sample_state *state, bool comparator_high)
{
    if (state->timed_out) {
        return BLHELI_S_BEMF_TIMED_OUT;
    }

    state->reads++;
    if (comparator_high != state->expected_high) {
        state->demag_detected = false;
        return BLHELI_S_BEMF_REJECTED;
    }
    if (state->reads < state->required) {
        return BLHELI_S_BEMF_WAITING;
    }
    return state->demag_detected ? BLHELI_S_BEMF_ACCEPTED
                                 : BLHELI_S_BEMF_REJECTED;
}
