#include "core/zero_crossing.h"

void blheli_s_zero_crossing_begin(
    struct blheli_s_zero_crossing_state *state,
    uint32_t now, uint32_t scan_delay, uint32_t timeout)
{
    *state = (struct blheli_s_zero_crossing_state){
        .scan_deadline = now + scan_delay,
        .timeout_deadline = now + timeout,
        .scan_started = false,
        .timed_out = false
    };
}

enum blheli_s_zero_crossing_result blheli_s_zero_crossing_update(
    struct blheli_s_zero_crossing_state *state, uint32_t now,
    bool comparator_qualified)
{
    if (state->timed_out) {
        return BLHELI_S_ZERO_CROSSING_TIMED_OUT;
    }
    if (!state->scan_started) {
        if ((int32_t)(now - state->scan_deadline) < 0) {
            return BLHELI_S_ZERO_CROSSING_SCANNING;
        }
        state->scan_started = true;
    }
    if (comparator_qualified) {
        return BLHELI_S_ZERO_CROSSING_READY;
    }
    if ((int32_t)(now - state->timeout_deadline) >= 0) {
        state->timed_out = true;
        return BLHELI_S_ZERO_CROSSING_TIMED_OUT;
    }
    return BLHELI_S_ZERO_CROSSING_SCANNING;
}
