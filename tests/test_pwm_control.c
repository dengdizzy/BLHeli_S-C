#include <assert.h>

#include "core/pwm_control.h"

void test_pwm_control(void)
{
    struct blheli_s_pwm_request request;
    const struct blheli_s_pwm_parameters parameters = {
        .pwm_limit = 100u,
        .pwm_limit_by_rpm = 80u,
        .requested_pwm = 0u,
        .clock_48mhz = false,
        .fet_on_delay = 2u
    };

    blheli_s_pwm_build_request(&parameters, &request);
    assert(request.power_compare == 0x02bfu);
    assert(request.damping_compare == 0x02bdu);
    assert(request.damping_enabled);

    {
        const struct blheli_s_pwm_parameters high_clock = {
            .pwm_limit = 255u,
            .pwm_limit_by_rpm = 255u,
            .requested_pwm = 0u,
            .clock_48mhz = true,
            .fet_on_delay = 200u
        };
        blheli_s_pwm_build_request(&high_clock, &request);
        assert(request.power_compare == 0x0007u);
        assert(request.damping_compare == 0u);
    }
}
