#ifndef BLHELI_S_PROTECTION_H
#define BLHELI_S_PROTECTION_H

#include <stdbool.h>
#include <stdint.h>

#define BLHELI_S_PROTECTION_TEMP_RATE 8u
#define BLHELI_S_PROTECTION_MAX_ACTIONS 8u

enum blheli_s_protection_action_kind {
    BLHELI_S_PROTECTION_ACTION_INCREMENT_ADC_COUNT,
    BLHELI_S_PROTECTION_ACTION_START_ADC_CONVERSION,
    BLHELI_S_PROTECTION_ACTION_WAIT_FOR_ADC_COMPLETE,
    BLHELI_S_PROTECTION_ACTION_READ_ADC_RESULT,
    BLHELI_S_PROTECTION_ACTION_STOP_ADC,
    BLHELI_S_PROTECTION_ACTION_RESET_ADC_COUNT,
    BLHELI_S_PROTECTION_ACTION_UPDATE_TEMPERATURE_AVERAGE,
    BLHELI_S_PROTECTION_ACTION_RECOVER_PWM_LIMIT,
    BLHELI_S_PROTECTION_ACTION_APPLY_TEMPERATURE_PWM_LIMIT
};

struct blheli_s_protection_action_trace {
    uint8_t action_count;
    enum blheli_s_protection_action_kind actions[BLHELI_S_PROTECTION_MAX_ACTIONS];
};

struct blheli_s_protection_descriptor {
    uint8_t adc_conversion_count;
    uint16_t adc_sample;
    uint8_t current_average_temperature;
    bool temperature_protection_enabled;
    uint8_t temperature_protection_limit;
    uint8_t pwm_limit;
    bool adc_conversion_complete;
};

struct blheli_s_protection_decision {
    uint8_t adc_conversion_count;
    uint8_t current_average_temperature;
    uint8_t pwm_limit;
    bool starts_adc_conversion;
    bool waits_for_adc_complete;
    bool reads_adc_result;
    bool stops_adc;
    bool resets_adc_conversion_count;
    bool updates_temperature_average;
    bool updates_pwm_limit;
    bool applies_temperature_limit;
    bool recovers_pwm_limit;
    bool adc_sample_byte_mapping_unknown;
    struct blheli_s_protection_action_trace trace;
};

uint8_t blheli_s_update_temperature_average(uint8_t average,
                                             uint16_t sample);
uint8_t blheli_s_temperature_pwm_limit(uint8_t average,
                                       uint8_t protection_limit);
void blheli_s_protection_start_adc_conversion(
    struct blheli_s_protection_decision *decision);
void blheli_s_protection_evaluate(
    const struct blheli_s_protection_descriptor *descriptor,
    struct blheli_s_protection_decision *decision);
uint8_t blheli_s_low_rpm_pwm_limit(uint16_t period_four_x,
                                   uint8_t slope, uint8_t startup_limit,
                                   bool enabled, bool initial_run);
uint8_t blheli_s_high_rpm_pwm_limit(uint16_t period_four_x,
                                    uint8_t current_limit,
                                    bool clock_48mhz);

#endif
