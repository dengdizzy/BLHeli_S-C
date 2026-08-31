#ifndef BLHELI_S_ZERO_CROSSING_H
#define BLHELI_S_ZERO_CROSSING_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_zero_crossing_result {
    BLHELI_S_ZERO_CROSSING_SCANNING,
    BLHELI_S_ZERO_CROSSING_READY,
    BLHELI_S_ZERO_CROSSING_TIMED_OUT
};

struct blheli_s_zero_crossing_state {
    uint32_t scan_deadline;
    uint32_t timeout_deadline;
    bool scan_started;
    bool timed_out;
};

void blheli_s_zero_crossing_begin(
    struct blheli_s_zero_crossing_state *state,
    uint32_t now, uint32_t scan_delay, uint32_t timeout);
enum blheli_s_zero_crossing_result blheli_s_zero_crossing_update(
    struct blheli_s_zero_crossing_state *state, uint32_t now,
    bool comparator_qualified);

#endif
