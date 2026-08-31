#include "core/zero_crossing.h"

static bool deadline_reached(uint32_t now, uint32_t deadline)
{
    return (int32_t)(now - deadline) >= 0;
}

uint32_t blheli_s_zero_crossing_timeout_duration(
    const struct blheli_s_zero_crossing_descriptor *descriptor)
{
    uint16_t timeout;

    if (!descriptor->startup_phase && !descriptor->initial_run_phase) {
        return descriptor->base_timeout;
    }

    timeout = descriptor->commutation_period_four_x;
    timeout >>= 1u;
    if (!descriptor->clock_48mhz) {
        timeout >>= 1u;
    }
    if (descriptor->startup_phase) {
        timeout = (uint16_t)(timeout + 0x4000u);
    }
    return timeout;
}

void blheli_s_zero_crossing_begin(
    struct blheli_s_zero_crossing_state *state,
    uint32_t now, uint32_t scan_delay, uint32_t timeout)
{
    const struct blheli_s_zero_crossing_descriptor descriptor = {
        .scan_delay = scan_delay,
        .base_timeout = timeout,
        .commutation_period_four_x = 0u,
        .startup_timeout_extensions = 2u,
        .startup_phase = false,
        .initial_run_phase = false,
        .clock_48mhz = false
    };

    blheli_s_zero_crossing_begin_from_descriptor(state, now, &descriptor);
}

void blheli_s_zero_crossing_begin_from_descriptor(
    struct blheli_s_zero_crossing_state *state, uint32_t now,
    const struct blheli_s_zero_crossing_descriptor *descriptor)
{
    uint32_t timeout = blheli_s_zero_crossing_timeout_duration(descriptor);

    *state = (struct blheli_s_zero_crossing_state){
        .scan_deadline = now + descriptor->scan_delay,
        .timeout_deadline = now + timeout,
        .timeout_duration = timeout,
        .startup_timeout_countdown =
            descriptor->startup_timeout_extensions == 0u
                ? 2u : descriptor->startup_timeout_extensions,
        .comparator_read_count = 0u,
        .scan_started = false,
        .timed_out = false,
        .startup_phase = descriptor->startup_phase,
        .initial_run_phase = descriptor->initial_run_phase,
        .timer_action = BLHELI_S_ZERO_CROSSING_TIMER_WAIT_SCAN
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
        if (!deadline_reached(now, state->scan_deadline)) {
            return BLHELI_S_ZERO_CROSSING_SCANNING;
        }
        state->scan_started = true;
        state->startup_timeout_countdown = 2u;
        state->timer_action = BLHELI_S_ZERO_CROSSING_TIMER_SET_TIMEOUT;
    }
    state->comparator_read_count++;
    if (comparator_qualified) {
        return BLHELI_S_ZERO_CROSSING_READY;
    }
    if (deadline_reached(now, state->timeout_deadline)) {
        if (state->startup_phase && state->comparator_read_count != 0u &&
            state->startup_timeout_countdown > 1u) {
            state->startup_timeout_countdown--;
            state->timeout_deadline = now + state->timeout_duration;
            state->timer_action =
                BLHELI_S_ZERO_CROSSING_TIMER_EXTEND_TIMEOUT;
            return BLHELI_S_ZERO_CROSSING_SCANNING;
        }
        state->timed_out = true;
        return BLHELI_S_ZERO_CROSSING_TIMED_OUT;
    }
    return BLHELI_S_ZERO_CROSSING_SCANNING;
}
