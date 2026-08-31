#ifndef BLHELI_S_PROTECTION_H
#define BLHELI_S_PROTECTION_H

#include <stdbool.h>
#include <stdint.h>

uint8_t blheli_s_update_temperature_average(uint8_t average,
                                             uint16_t sample);
uint8_t blheli_s_temperature_pwm_limit(uint8_t average,
                                       uint8_t protection_limit);
uint8_t blheli_s_low_rpm_pwm_limit(uint16_t period_four_x,
                                   uint8_t slope, uint8_t startup_limit,
                                   bool enabled, bool initial_run);
uint8_t blheli_s_high_rpm_pwm_limit(uint16_t period_four_x,
                                    uint8_t current_limit,
                                    bool clock_48mhz);

#endif
