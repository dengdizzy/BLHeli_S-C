#include <assert.h>

#include "core/protection.h"

void test_protection(void)
{
    struct blheli_s_protection_descriptor descriptor = {
        .adc_conversion_count = 6u,
        .adc_sample = 0x4000u,
        .current_average_temperature = 64u,
        .temperature_protection_enabled = true,
        .temperature_protection_limit = 70u,
        .pwm_limit = 100u,
        .adc_conversion_complete = true
    };
    struct blheli_s_protection_decision decision;

    assert(blheli_s_update_temperature_average(10u, 0u) == 9u);
    assert(blheli_s_update_temperature_average(10u, 0x0b00u) == 11u);
    assert(blheli_s_update_temperature_average(10u, 0x0a00u) == 10u);
    assert(blheli_s_update_temperature_average(0u, 0u) == 0u);
    assert(blheli_s_temperature_pwm_limit(50u, 49u) == 192u);
    assert(blheli_s_temperature_pwm_limit(54u, 49u) == 128u);
    assert(blheli_s_temperature_pwm_limit(59u, 49u) == 64u);
    assert(blheli_s_temperature_pwm_limit(65u, 49u) == 0u);
    assert(blheli_s_low_rpm_pwm_limit(0u, 2u, 40u, true, false) == 255u);
    assert(blheli_s_high_rpm_pwm_limit(100u, 10u, true) == 11u);

    blheli_s_protection_start_adc_conversion(&decision);
    assert(decision.starts_adc_conversion);
    assert(decision.trace.action_count == 1u);
    assert(decision.trace.actions[0] ==
           BLHELI_S_PROTECTION_ACTION_START_ADC_CONVERSION);

    blheli_s_protection_evaluate(&descriptor, &decision);
    assert(decision.adc_conversion_count == 7u);
    assert(decision.pwm_limit == 116u);
    assert(decision.recovers_pwm_limit);
    assert(decision.updates_pwm_limit);
    assert(!decision.waits_for_adc_complete);
    assert(decision.trace.action_count == 2u);
    assert(decision.trace.actions[0] ==
           BLHELI_S_PROTECTION_ACTION_INCREMENT_ADC_COUNT);
    assert(decision.trace.actions[1] ==
           BLHELI_S_PROTECTION_ACTION_RECOVER_PWM_LIMIT);

    descriptor.pwm_limit = 250u;
    blheli_s_protection_evaluate(&descriptor, &decision);
    assert(decision.pwm_limit == 255u);

    descriptor.adc_conversion_count = 7u;
    descriptor.pwm_limit = 200u;
    descriptor.adc_conversion_complete = false;
    blheli_s_protection_evaluate(&descriptor, &decision);
    assert(decision.adc_conversion_count == 8u);
    assert(decision.waits_for_adc_complete);
    assert(!decision.reads_adc_result);
    assert(!decision.updates_pwm_limit);
    assert(decision.trace.action_count == 2u);
    assert(decision.trace.actions[1] ==
           BLHELI_S_PROTECTION_ACTION_WAIT_FOR_ADC_COMPLETE);

    descriptor.adc_conversion_complete = true;
    descriptor.temperature_protection_enabled = false;
    blheli_s_protection_evaluate(&descriptor, &decision);
    assert(decision.adc_conversion_count == 0u);
    assert(decision.reads_adc_result);
    assert(decision.stops_adc);
    assert(decision.resets_adc_conversion_count);
    assert(!decision.updates_temperature_average);
    assert(decision.pwm_limit == 200u);
    assert(decision.trace.actions[2] ==
           BLHELI_S_PROTECTION_ACTION_READ_ADC_RESULT);
    assert(decision.trace.actions[3] ==
           BLHELI_S_PROTECTION_ACTION_STOP_ADC);
    assert(decision.trace.actions[4] ==
           BLHELI_S_PROTECTION_ACTION_RESET_ADC_COUNT);

    descriptor.temperature_protection_enabled = true;
    descriptor.current_average_temperature = 70u;
    descriptor.temperature_protection_limit = 70u;
    descriptor.adc_sample = 0x7100u;
    blheli_s_protection_evaluate(&descriptor, &decision);
    assert(decision.current_average_temperature == 71u);
    assert(decision.updates_temperature_average);
    assert(decision.applies_temperature_limit);
    assert(decision.pwm_limit == 192u);
    assert(decision.adc_sample_byte_mapping_unknown);
    assert(decision.trace.action_count == 7u);
    assert(decision.trace.actions[5] ==
           BLHELI_S_PROTECTION_ACTION_UPDATE_TEMPERATURE_AVERAGE);
    assert(decision.trace.actions[6] ==
           BLHELI_S_PROTECTION_ACTION_APPLY_TEMPERATURE_PWM_LIMIT);

    descriptor.current_average_temperature = 74u;
    descriptor.adc_sample = 0x7500u;
    blheli_s_protection_evaluate(&descriptor, &decision);
    assert(decision.pwm_limit == 128u);

    descriptor.current_average_temperature = 79u;
    descriptor.adc_sample = 0x8000u;
    blheli_s_protection_evaluate(&descriptor, &decision);
    assert(decision.pwm_limit == 64u);

    descriptor.current_average_temperature = 84u;
    descriptor.adc_sample = 0x8500u;
    blheli_s_protection_evaluate(&descriptor, &decision);
    assert(decision.pwm_limit == 0u);
}
