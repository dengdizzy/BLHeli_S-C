#include <assert.h>

#include "config/blheli_s_config.h"

void test_config(void)
{
    struct blheli_s_config config;
    struct blheli_s_decoded_config decoded;

    blheli_s_config_defaults(&config);
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.startup_power_decoded == 0x40u);
    assert(decoded.low_rpm_power_slope == 9u);
    assert(decoded.demag_power_off_threshold == 160u);
    assert(decoded.temperature_protection_limit == 103u);
    assert(!decoded.bidirectional);
    assert(blheli_s_scale_throttle_calibration(37u, false) == 1173u);
    assert(blheli_s_scale_throttle_calibration(37u, true) == 2346u);

    config.startup_power = 0u;
    assert(!blheli_s_decode_config(&config, &decoded));
    config.startup_power = 1u;
    config.direction = BLHELI_S_DIRECTION_BIDIRECTIONAL;
    config.demag_compensation = BLHELI_S_DEMAG_HIGH;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.bidirectional && decoded.direction_reversed);
    assert(decoded.demag_power_off_threshold == 130u);
}
