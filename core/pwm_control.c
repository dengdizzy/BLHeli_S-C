#include "core/pwm_control.h"

static uint16_t blheli_s_pwm_compare(uint8_t limit, bool clock_48mhz)
{
    uint16_t scaled = (uint16_t)limit * (clock_48mhz ? 8u : 4u);
    uint16_t high_mask = clock_48mhz ? 7u : 3u;

    return (uint16_t)((uint16_t)~scaled &
                      (uint16_t)(0x00ffu | (uint16_t)(high_mask << 8u)));
}

void blheli_s_pwm_build_request(
    const struct blheli_s_pwm_parameters *parameters,
    struct blheli_s_pwm_request *request)
{
    uint8_t limit = parameters->pwm_limit;
    uint16_t power_compare;
    uint16_t damping_compare;
    uint16_t delay;

    if (parameters->pwm_limit_by_rpm < limit) {
        limit = parameters->pwm_limit_by_rpm;
    }

    power_compare = blheli_s_pwm_compare(limit, parameters->clock_48mhz);
    damping_compare = power_compare;
    delay = (uint16_t)parameters->fet_on_delay *
            (parameters->clock_48mhz ? 2u : 1u);
    if (delay > damping_compare) {
        damping_compare = 0u;
    } else {
        damping_compare = (uint16_t)(damping_compare - delay);
    }

    *request = (struct blheli_s_pwm_request){
        .power_compare = power_compare,
        .damping_compare = damping_compare,
        .damping_enabled = parameters->fet_on_delay != 0u
    };
}
