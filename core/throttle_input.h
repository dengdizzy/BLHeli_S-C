#ifndef BLHELI_S_THROTTLE_INPUT_H
#define BLHELI_S_THROTTLE_INPUT_H

#include <stdbool.h>
#include <stdint.h>

#include "core/throttle_calibration.h"

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
    bool direction_reversed;
    bool stop;
    bool outside_range;
};

struct blheli_s_throttle_input_result blheli_s_process_throttle(
    uint16_t capture, const struct blheli_s_throttle_input_config *config);

#endif
