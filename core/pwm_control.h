#ifndef BLHELI_S_PWM_CONTROL_H
#define BLHELI_S_PWM_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

struct blheli_s_pwm_request {
    uint16_t power_compare;
    uint16_t damping_compare;
    bool damping_enabled;
};

struct blheli_s_pwm_parameters {
    uint8_t pwm_limit;
    uint8_t pwm_limit_by_rpm;
    uint8_t requested_pwm;
    bool clock_48mhz;
    uint8_t fet_on_delay;
};

void blheli_s_pwm_build_request(
    const struct blheli_s_pwm_parameters *parameters,
    struct blheli_s_pwm_request *request);

#endif
