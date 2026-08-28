#ifndef BLHELI_S_THROTTLE_CALIBRATION_H
#define BLHELI_S_THROTTLE_CALIBRATION_H

#include <stdbool.h>
#include <stdint.h>

struct blheli_s_throttle_gain {
    uint8_t gain;
    uint8_t multiplier;
};

struct blheli_s_throttle_gains {
    struct blheli_s_throttle_gain forward;
    struct blheli_s_throttle_gain reverse;
};

struct blheli_s_throttle_gain blheli_s_find_throttle_gain(
    uint16_t minimum, uint16_t maximum, uint8_t deadband);
struct blheli_s_throttle_gains blheli_s_find_throttle_gains(
    uint8_t minimum, uint8_t center, uint8_t maximum,
    bool bidirectional, bool full_range);

#endif
