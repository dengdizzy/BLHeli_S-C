#include "core/protection.h"

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
