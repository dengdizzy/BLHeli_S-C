#include <assert.h>

#include "core/protection.h"

void test_protection(void)
{
    assert(blheli_s_update_temperature_average(10u, 0u) == 9u);
    assert(blheli_s_update_temperature_average(10u, 0x0b00u) == 11u);
    assert(blheli_s_temperature_pwm_limit(50u, 49u) == 192u);
    assert(blheli_s_temperature_pwm_limit(65u, 49u) == 0u);
    assert(blheli_s_low_rpm_pwm_limit(0u, 2u, 40u, true, false) == 255u);
    assert(blheli_s_high_rpm_pwm_limit(100u, 10u, true) == 11u);
}
