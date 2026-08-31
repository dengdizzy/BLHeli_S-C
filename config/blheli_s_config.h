#ifndef BLHELI_S_CONFIG_H
#define BLHELI_S_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_direction {
    BLHELI_S_DIRECTION_NORMAL = 1,
    BLHELI_S_DIRECTION_REVERSED = 2,
    BLHELI_S_DIRECTION_BIDIRECTIONAL = 3,
    BLHELI_S_DIRECTION_BIDIRECTIONAL_REVERSED = 4
};

enum blheli_s_demag_compensation {
    BLHELI_S_DEMAG_DISABLED = 1,
    BLHELI_S_DEMAG_LOW = 2,
    BLHELI_S_DEMAG_HIGH = 3
};

struct blheli_s_config {
    uint8_t startup_power;
    uint8_t direction;
    uint8_t commutation_timing;
    uint8_t min_throttle;
    uint8_t max_throttle;
    uint8_t center_throttle;
    uint8_t beep_strength;
    uint8_t beacon_strength;
    uint8_t beacon_delay;
    uint8_t demag_compensation;
    uint8_t temperature_protection;
    uint8_t power_protection;
    uint8_t brake_on_stop;
    uint8_t led_control;
    uint8_t enable_tx_programming;
};

struct blheli_s_decoded_config {
    bool bidirectional;
    bool direction_reversed;
    bool bidirectional_reversed;
    bool decoded_direction_flags;
    bool decoded_startup_power;
    bool decoded_low_rpm_power_slope;
    bool decoded_demag_threshold;
    bool decoded_temperature_limit;
    bool switch_power_off_intent;
    bool unknown_fields_preserved;
    uint8_t startup_power_decoded;
    uint8_t low_rpm_power_slope;
    uint8_t demag_power_off_threshold;
    uint8_t temperature_protection_limit;
};

void blheli_s_config_defaults(struct blheli_s_config *config);
bool blheli_s_decode_config(const struct blheli_s_config *config,
                            struct blheli_s_decoded_config *decoded);
uint16_t blheli_s_scale_throttle_calibration(uint8_t value, bool clock_48mhz);

#endif
