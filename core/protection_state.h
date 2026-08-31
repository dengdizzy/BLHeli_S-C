#ifndef BLHELI_S_PROTECTION_STATE_H
#define BLHELI_S_PROTECTION_STATE_H

#include <stdint.h>

struct blheli_s_protection_state {
    uint8_t demag_detected_metric;
    uint8_t demag_power_off_threshold;
    uint8_t low_rpm_power_slope;
    uint8_t adc_conversion_count;
    uint8_t current_average_temperature;
    uint8_t temperature_protection_limit;
    uint8_t comparator_read_count;
    uint8_t comparator_timed_out;
    uint8_t demag_detected;
};

#endif
