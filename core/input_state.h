#ifndef BLHELI_S_INPUT_STATE_H
#define BLHELI_S_INPUT_STATE_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_input_format {
    BLHELI_S_INPUT_FORMAT_UNKNOWN,
    BLHELI_S_INPUT_FORMAT_PPM,
    BLHELI_S_INPUT_FORMAT_ONESHOT125,
    BLHELI_S_INPUT_FORMAT_ONESHOT42,
    BLHELI_S_INPUT_FORMAT_MULTISHOT,
    BLHELI_S_INPUT_FORMAT_DSHOT
};

struct blheli_s_input_state {
    enum blheli_s_input_format format;
    uint8_t outside_range_count;
    uint8_t timeout_countdown;
    uint8_t updated;
    uint8_t dshot_command;
    uint8_t dshot_command_count;
    uint16_t minimum_throttle;
    uint16_t center_throttle;
    uint16_t maximum_throttle;
    uint8_t throttle_gain;
    uint8_t throttle_gain_multiplier;
    uint8_t reverse_throttle_gain;
    uint8_t reverse_throttle_gain_multiplier;
    bool full_range;
};

#endif
