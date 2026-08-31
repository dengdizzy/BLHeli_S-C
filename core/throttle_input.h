#ifndef BLHELI_S_THROTTLE_INPUT_H
#define BLHELI_S_THROTTLE_INPUT_H

#include <stdbool.h>
#include <stdint.h>

#include "core/throttle_calibration.h"

enum blheli_s_throttle_protocol {
    BLHELI_S_THROTTLE_PROTOCOL_PPM,
    BLHELI_S_THROTTLE_PROTOCOL_ONESHOT125,
    BLHELI_S_THROTTLE_PROTOCOL_ONESHOT42,
    BLHELI_S_THROTTLE_PROTOCOL_MULTISHOT,
    BLHELI_S_THROTTLE_PROTOCOL_DSHOT
};

struct blheli_s_throttle_input_config {
    uint16_t minimum;
    uint16_t center;
    uint16_t maximum;
    struct blheli_s_throttle_gain forward_gain;
    struct blheli_s_throttle_gain reverse_gain;
    bool bidirectional;
    bool full_range;
    bool bidirectional_reversed;
};

struct blheli_s_throttle_input_result {
    uint16_t value;
    uint16_t scaled_capture;
    uint16_t new_rcp;
    bool direction_reversed;
    bool stop;
    bool outside_range;
    bool new_rcp_updated;
    bool new_rcp_cleared;
    bool startup_boost_applied;
    bool timeout_reloaded;
};

struct blheli_s_throttle_input_state {
    uint8_t outside_range_count;
    uint8_t stop_count;
    uint8_t timeout_countdown;
    uint16_t new_rcp;
    bool rcp_updated;
};

struct blheli_s_throttle_input_descriptor {
    enum blheli_s_throttle_protocol protocol;
    struct blheli_s_throttle_input_config config;
    uint16_t raw_capture;
    uint8_t startup_pwm_limit_begin;
    uint8_t stall_count;
    bool clock_48mhz;
    bool startup_phase;
    bool initial_run_phase;
    bool motor_started;
};

uint16_t blheli_s_throttle_scale_capture(
    enum blheli_s_throttle_protocol protocol, uint16_t raw_capture,
    bool clock_48mhz);
void blheli_s_throttle_input_state_init(
    struct blheli_s_throttle_input_state *state);
void blheli_s_throttle_input_timer_tick(
    struct blheli_s_throttle_input_state *state);
struct blheli_s_throttle_input_result
blheli_s_process_throttle_from_descriptor(
    const struct blheli_s_throttle_input_descriptor *descriptor,
    struct blheli_s_throttle_input_state *state);
struct blheli_s_throttle_input_result blheli_s_process_throttle(
    uint16_t capture, const struct blheli_s_throttle_input_config *config);

#endif
