#ifndef BLHELI_S_ZERO_CROSSING_H
#define BLHELI_S_ZERO_CROSSING_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_zero_crossing_result {
    BLHELI_S_ZERO_CROSSING_SCANNING,
    BLHELI_S_ZERO_CROSSING_READY,
    BLHELI_S_ZERO_CROSSING_TIMED_OUT
};

enum blheli_s_zero_crossing_timer_action {
    BLHELI_S_ZERO_CROSSING_TIMER_NONE,
    BLHELI_S_ZERO_CROSSING_TIMER_WAIT_SCAN,
    BLHELI_S_ZERO_CROSSING_TIMER_SET_TIMEOUT,
    BLHELI_S_ZERO_CROSSING_TIMER_EXTEND_TIMEOUT
};

struct blheli_s_zero_crossing_descriptor {
    uint32_t scan_delay;
    uint32_t base_timeout;
    uint16_t commutation_period_four_x;
    uint8_t startup_timeout_extensions;
    bool startup_phase;
    bool initial_run_phase;
    bool clock_48mhz;
};

struct blheli_s_zero_crossing_state {
    uint32_t scan_deadline;
    uint32_t timeout_deadline;
    uint32_t timeout_duration;
    uint8_t startup_timeout_countdown;
    uint8_t comparator_read_count;
    bool scan_started;
    bool timed_out;
    bool startup_phase;
    bool initial_run_phase;
    enum blheli_s_zero_crossing_timer_action timer_action;
};

uint32_t blheli_s_zero_crossing_timeout_duration(
    const struct blheli_s_zero_crossing_descriptor *descriptor);
void blheli_s_zero_crossing_begin(
    struct blheli_s_zero_crossing_state *state,
    uint32_t now, uint32_t scan_delay, uint32_t timeout);
void blheli_s_zero_crossing_begin_from_descriptor(
    struct blheli_s_zero_crossing_state *state, uint32_t now,
    const struct blheli_s_zero_crossing_descriptor *descriptor);
enum blheli_s_zero_crossing_result blheli_s_zero_crossing_update(
    struct blheli_s_zero_crossing_state *state, uint32_t now,
    bool comparator_qualified);

#endif
