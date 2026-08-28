#include "core/throttle_calibration.h"

struct blheli_s_throttle_gain blheli_s_find_throttle_gain(
    uint16_t minimum, uint16_t maximum, uint8_t deadband)
{
    uint16_t range = maximum > (uint16_t)deadband
        ? (uint16_t)(maximum - (uint16_t)deadband) : 0u;
    uint8_t multiplier = 0u;

    range = range > minimum ? (uint16_t)(range - minimum) : 0u;
    if (range < 35u) {
        range = 35u;
    } else if (range > 511u) {
        range = 511u;
    }

    for (;;) {
        for (uint16_t gain = 1u; gain <= 255u; ++gain) {
            if (range * gain >= 124u) {
                return (struct blheli_s_throttle_gain){
                    .gain = (uint8_t)gain, .multiplier = multiplier
                };
            }
        }
        range <<= 1u;
        ++multiplier;
    }
}

struct blheli_s_throttle_gains blheli_s_find_throttle_gains(
    uint8_t minimum, uint8_t center, uint8_t maximum,
    bool bidirectional, bool full_range)
{
    if (full_range) {
        struct blheli_s_throttle_gain gain =
            blheli_s_find_throttle_gain(0u, 255u, 0u);
        return (struct blheli_s_throttle_gains){ .forward = gain, .reverse = gain };
    }

    if (!bidirectional) {
        struct blheli_s_throttle_gain gain =
            blheli_s_find_throttle_gain(minimum, maximum, 0u);
        return (struct blheli_s_throttle_gains){ .forward = gain, .reverse = gain };
    }

    return (struct blheli_s_throttle_gains){
        .forward = blheli_s_find_throttle_gain(
            (uint16_t)center << 1u, (uint16_t)maximum << 1u, 10u),
        .reverse = blheli_s_find_throttle_gain(
            (uint16_t)minimum << 1u, (uint16_t)center << 1u, 10u)
    };
}
