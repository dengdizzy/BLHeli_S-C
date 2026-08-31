#include "core/throttle_input.h"

#include "core/throttle_calibration.h"

static uint16_t apply_gain(uint16_t value, struct blheli_s_throttle_gain gain)
{
    uint32_t scaled = (uint32_t)value * gain.gain;

    scaled >>= 7u;
    scaled <<= gain.multiplier;
    return scaled > UINT16_MAX ? UINT16_MAX : (uint16_t)scaled;
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

    if (capture < 900u || capture > 2200u) {
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
