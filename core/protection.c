#include "core/protection.h"

static void append_protection_action(
    struct blheli_s_protection_action_trace *trace,
    enum blheli_s_protection_action_kind action)
{
    if (trace->action_count < BLHELI_S_PROTECTION_MAX_ACTIONS) {
        trace->actions[trace->action_count] = action;
        trace->action_count++;
    }
}

uint8_t blheli_s_update_temperature_average(uint8_t average,
                                             uint16_t sample)
{
    uint8_t value = (uint8_t)(sample >> 8u);

    if (value == 0u) {
        return average == 0u ? 0u : (uint8_t)(average - 1u);
    }
    if (value > average) {
        return average == UINT8_MAX ? 0u : (uint8_t)(average + 1u);
    }
    if (value < average) {
        return (uint8_t)(average - 1u);
    }
    return average;
}

uint8_t blheli_s_temperature_pwm_limit(uint8_t average,
                                       uint8_t protection_limit)
{
    if (average < protection_limit) {
        return 255u;
    }
    if (average < (uint8_t)(protection_limit + 5u)) {
        return 192u;
    }
    if (average < (uint8_t)(protection_limit + 10u)) {
        return 128u;
    }
    if (average < (uint8_t)(protection_limit + 15u)) {
        return 64u;
    }
    return 0u;
}

void blheli_s_protection_start_adc_conversion(
    struct blheli_s_protection_decision *decision)
{
    *decision = (struct blheli_s_protection_decision){
        .starts_adc_conversion = true
    };
    append_protection_action(&decision->trace,
                             BLHELI_S_PROTECTION_ACTION_START_ADC_CONVERSION);
}

void blheli_s_protection_evaluate(
    const struct blheli_s_protection_descriptor *descriptor,
    struct blheli_s_protection_decision *decision)
{
    uint8_t next_count = (uint8_t)(descriptor->adc_conversion_count + 1u);

    *decision = (struct blheli_s_protection_decision){
        .adc_conversion_count = next_count,
        .current_average_temperature = descriptor->current_average_temperature,
        .pwm_limit = descriptor->pwm_limit,
        .adc_sample_byte_mapping_unknown = true
    };

    append_protection_action(&decision->trace,
                             BLHELI_S_PROTECTION_ACTION_INCREMENT_ADC_COUNT);

    if (next_count < BLHELI_S_PROTECTION_TEMP_RATE) {
        uint16_t recovered = (uint16_t)(descriptor->pwm_limit + 16u);

        if (recovered > UINT8_MAX) {
            recovered = UINT8_MAX;
        }
        decision->pwm_limit = (uint8_t)recovered;
        decision->updates_pwm_limit = true;
        decision->recovers_pwm_limit = true;
        append_protection_action(&decision->trace,
                                 BLHELI_S_PROTECTION_ACTION_RECOVER_PWM_LIMIT);
        return;
    }

    decision->waits_for_adc_complete = true;
    append_protection_action(&decision->trace,
                             BLHELI_S_PROTECTION_ACTION_WAIT_FOR_ADC_COMPLETE);

    if (!descriptor->adc_conversion_complete) {
        return;
    }

    decision->reads_adc_result = true;
    decision->stops_adc = true;
    decision->resets_adc_conversion_count = true;
    decision->adc_conversion_count = 0u;
    append_protection_action(&decision->trace,
                             BLHELI_S_PROTECTION_ACTION_READ_ADC_RESULT);
    append_protection_action(&decision->trace,
                             BLHELI_S_PROTECTION_ACTION_STOP_ADC);
    append_protection_action(&decision->trace,
                             BLHELI_S_PROTECTION_ACTION_RESET_ADC_COUNT);

    if (!descriptor->temperature_protection_enabled) {
        return;
    }

    decision->current_average_temperature =
        blheli_s_update_temperature_average(
            descriptor->current_average_temperature, descriptor->adc_sample);
    decision->updates_temperature_average = true;
    append_protection_action(
        &decision->trace,
        BLHELI_S_PROTECTION_ACTION_UPDATE_TEMPERATURE_AVERAGE);

    if (decision->current_average_temperature >=
        descriptor->temperature_protection_limit) {
        uint8_t limit = blheli_s_temperature_pwm_limit(
            decision->current_average_temperature,
            descriptor->temperature_protection_limit);

        decision->pwm_limit = limit;
        decision->updates_pwm_limit = true;
        decision->applies_temperature_limit = true;
        append_protection_action(
            &decision->trace,
            BLHELI_S_PROTECTION_ACTION_APPLY_TEMPERATURE_PWM_LIMIT);
    }
}

uint8_t blheli_s_low_rpm_pwm_limit(uint16_t period_four_x,
                                   uint8_t slope, uint8_t startup_limit,
                                   bool enabled, bool initial_run)
{
    uint16_t limit;

    if (!enabled || period_four_x == 0u) {
        return 255u;
    }
    limit = (uint16_t)(255u / (period_four_x >> 8u));
    limit *= initial_run ? 5u : slope;
    if (limit < startup_limit) {
        limit = startup_limit;
    }
    return limit > 255u ? 255u : (uint8_t)limit;
}

uint8_t blheli_s_high_rpm_pwm_limit(uint16_t period_four_x,
                                    uint8_t current_limit,
                                    bool clock_48mhz)
{
    uint16_t threshold = clock_48mhz ? 160u : 228u;

    if (period_four_x < threshold) {
        return current_limit == 255u ? 255u : (uint8_t)(current_limit + 1u);
    }
    return current_limit == 0u ? 0u : (uint8_t)(current_limit - 1u);
}
