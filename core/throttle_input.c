#include "core/throttle_input.h"

#include "core/throttle_calibration.h"

#define BLHELI_S_THROTTLE_OUTSIDE_RANGE_LIMIT 50u
#define BLHELI_S_THROTTLE_TIMEOUT_RELOAD 10u
#define BLHELI_S_THROTTLE_MIN_CAPTURE 900u
#define BLHELI_S_THROTTLE_MAX_CAPTURE 2235u

static uint16_t apply_gain(uint16_t value, struct blheli_s_throttle_gain gain)
{
    uint32_t scaled = (uint32_t)value * gain.gain;

    scaled >>= 7u;
    scaled <<= gain.multiplier;
    return scaled > UINT16_MAX ? UINT16_MAX : (uint16_t)scaled;
}

uint16_t blheli_s_throttle_scale_capture(
    enum blheli_s_throttle_protocol protocol, uint16_t raw_capture,
    bool clock_48mhz)
{
    uint32_t scaled = raw_capture;

    switch (protocol) {
    case BLHELI_S_THROTTLE_PROTOCOL_MULTISHOT:
        scaled = (uint32_t)raw_capture * 2u + (raw_capture >> 4u) +
                 (raw_capture >> 5u) + (clock_48mhz ? 0x0600u : 0x0300u);
        break;
    case BLHELI_S_THROTTLE_PROTOCOL_ONESHOT42:
        scaled = (uint32_t)raw_capture + (raw_capture >> 7u);
        break;
    case BLHELI_S_THROTTLE_PROTOCOL_ONESHOT125:
        scaled = ((uint32_t)raw_capture * 0x56u) >> 8u;
        break;
    case BLHELI_S_THROTTLE_PROTOCOL_PPM:
        scaled = ((uint32_t)raw_capture * 0x2bu) >> 10u;
        if (clock_48mhz) {
            scaled = ((uint32_t)raw_capture * 0x56u) >> 11u;
        }
        break;
    case BLHELI_S_THROTTLE_PROTOCOL_DSHOT:
        scaled = raw_capture;
        break;
    }

    return scaled > UINT16_MAX ? UINT16_MAX : (uint16_t)scaled;
}

void blheli_s_throttle_input_state_init(
    struct blheli_s_throttle_input_state *state)
{
    *state = (struct blheli_s_throttle_input_state){ 0 };
}

void blheli_s_throttle_input_timer_tick(
    struct blheli_s_throttle_input_state *state)
{
    if (state->timeout_countdown != 0u) {
        state->timeout_countdown--;
    }
    if (state->new_rcp == 0u) {
        if (state->stop_count != UINT8_MAX) {
            state->stop_count++;
        }
    } else {
        state->stop_count = 0u;
    }
}

struct blheli_s_throttle_input_result blheli_s_process_throttle(
    uint16_t capture, const struct blheli_s_throttle_input_config *config)
{
    struct blheli_s_throttle_input_result result = {
        .value = 0u,
        .direction_reversed = config->bidirectional_reversed,
        .stop = false,
        .outside_range = false
    };
    uint16_t base;
    struct blheli_s_throttle_gain gain = config->forward_gain;

    if (capture < BLHELI_S_THROTTLE_MIN_CAPTURE ||
        capture >= BLHELI_S_THROTTLE_MAX_CAPTURE) {
        result.outside_range = true;
        return result;
    }

    if (config->full_range) {
        base = capture > 1000u ? (uint16_t)(capture - 1000u) : 0u;
    } else if (config->bidirectional) {
        bool reverse = capture < config->center;
        uint16_t origin = reverse ? config->minimum : config->center;
        base = capture > origin ? (uint16_t)(capture - origin) : 0u;
        if (base <= 10u) {
            base = 0u;
        } else {
            base = (uint16_t)(base - 10u);
        }
        result.direction_reversed = reverse != config->bidirectional_reversed;
        gain = reverse ? config->reverse_gain : config->forward_gain;
    } else {
        base = capture > config->minimum
            ? (uint16_t)(capture - config->minimum) : 0u;
        if (capture < config->minimum) {
            result.stop = true;
        }
    }

    result.value = apply_gain(base, gain);
    if (result.value == 0u) {
        result.stop = true;
    }
    return result;
}

struct blheli_s_throttle_input_result
blheli_s_process_throttle_from_descriptor(
    const struct blheli_s_throttle_input_descriptor *descriptor,
    struct blheli_s_throttle_input_state *state)
{
    uint16_t scaled_capture = blheli_s_throttle_scale_capture(
        descriptor->protocol, descriptor->raw_capture,
        descriptor->clock_48mhz);
    struct blheli_s_throttle_input_result result =
        blheli_s_process_throttle(scaled_capture, &descriptor->config);

    result.scaled_capture = scaled_capture;
    if (result.outside_range) {
        if (state->outside_range_count != UINT8_MAX) {
            state->outside_range_count++;
        }
        state->timeout_countdown = BLHELI_S_THROTTLE_TIMEOUT_RELOAD;
        result.timeout_reloaded = true;
        result.new_rcp_cleared =
            state->outside_range_count >=
            BLHELI_S_THROTTLE_OUTSIDE_RANGE_LIMIT;
        if (result.new_rcp_cleared) {
            state->new_rcp = 0u;
            state->rcp_updated = true;
            result.new_rcp_updated = true;
        }
        result.new_rcp = state->new_rcp;
        return result;
    }

    if (state->outside_range_count != 0u) {
        state->outside_range_count--;
    }
    if ((descriptor->startup_phase || descriptor->initial_run_phase) &&
        !descriptor->motor_started && result.value != 0u) {
        uint16_t boost = descriptor->startup_pwm_limit_begin;

        if (descriptor->clock_48mhz) {
            boost = (uint16_t)(boost << 1u);
        }
        boost = (uint16_t)(boost + ((uint16_t)descriptor->stall_count << 4u));
        if (result.value < boost) {
            result.value = boost;
            result.startup_boost_applied = true;
        }
    }

    state->new_rcp = result.value;
    state->rcp_updated = true;
    state->timeout_countdown = BLHELI_S_THROTTLE_TIMEOUT_RELOAD;
    result.new_rcp = state->new_rcp;
    result.new_rcp_updated = true;
    result.timeout_reloaded = true;
    if (state->new_rcp == 0u) {
        if (state->stop_count != UINT8_MAX) {
            state->stop_count++;
        }
    } else {
        state->stop_count = 0u;
    }
    return result;
}
