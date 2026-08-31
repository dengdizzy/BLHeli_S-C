#include <assert.h>

#include "config/blheli_s_config.h"

void test_config(void)
{
    struct blheli_s_config config;
    struct blheli_s_decoded_config decoded;

    blheli_s_config_defaults(&config);
    assert(config.startup_power == 9u);
    assert(config.direction == BLHELI_S_DIRECTION_NORMAL);
    assert(config.commutation_timing == 3u);
    assert(config.min_throttle == 37u);
    assert(config.max_throttle == 208u);
    assert(config.center_throttle == 122u);
    assert(config.beep_strength == 40u);
    assert(config.beacon_strength == 80u);
    assert(config.beacon_delay == 4u);
    assert(config.demag_compensation == BLHELI_S_DEMAG_LOW);
    assert(config.temperature_protection == 7u);
    assert(config.power_protection == 1u);
    assert(config.brake_on_stop == 0u);
    assert(config.led_control == 0u);
    assert(config.enable_tx_programming == 1u);

    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.decoded_direction_flags);
    assert(decoded.decoded_startup_power);
    assert(decoded.decoded_low_rpm_power_slope);
    assert(decoded.decoded_demag_threshold);
    assert(decoded.decoded_temperature_limit);
    assert(decoded.switch_power_off_intent);
    assert(decoded.unknown_fields_preserved);
    assert(decoded.startup_power_decoded == 0x40u);
    assert(decoded.low_rpm_power_slope == 9u);
    assert(decoded.demag_power_off_threshold == 160u);
    assert(decoded.temperature_protection_limit == 103u);
    assert(!decoded.bidirectional);
    assert(blheli_s_scale_throttle_calibration(37u, false) == 1173u);
    assert(blheli_s_scale_throttle_calibration(37u, true) == 2346u);

    config.startup_power = 0u;
    assert(!blheli_s_decode_config(&config, &decoded));
    config.startup_power = 14u;
    assert(!blheli_s_decode_config(&config, &decoded));
    config.startup_power = 1u;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.startup_power_decoded == 0x04u);
    assert(decoded.low_rpm_power_slope == 2u);
    config.startup_power = 13u;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.startup_power_decoded == 0xc0u);
    assert(decoded.low_rpm_power_slope == 13u);

    config.startup_power = 9u;
    config.direction = BLHELI_S_DIRECTION_NORMAL;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(!decoded.bidirectional);
    assert(!decoded.direction_reversed);
    assert(!decoded.bidirectional_reversed);
    config.direction = BLHELI_S_DIRECTION_REVERSED;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(!decoded.bidirectional);
    assert(decoded.direction_reversed);
    assert(decoded.bidirectional_reversed);
    config.direction = BLHELI_S_DIRECTION_BIDIRECTIONAL;
    config.demag_compensation = BLHELI_S_DEMAG_HIGH;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.bidirectional && decoded.direction_reversed);
    assert(decoded.demag_power_off_threshold == 130u);
    config.direction = BLHELI_S_DIRECTION_BIDIRECTIONAL_REVERSED;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.bidirectional);
    assert(!decoded.direction_reversed);
    assert(!decoded.bidirectional_reversed);
    config.direction = 0u;
    assert(!blheli_s_decode_config(&config, &decoded));
    config.direction = 5u;
    assert(!blheli_s_decode_config(&config, &decoded));

    config.direction = BLHELI_S_DIRECTION_NORMAL;
    config.demag_compensation = BLHELI_S_DEMAG_DISABLED;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.demag_power_off_threshold == 255u);
    config.demag_compensation = 0u;
    assert(!blheli_s_decode_config(&config, &decoded));
    config.demag_compensation = 4u;
    assert(!blheli_s_decode_config(&config, &decoded));

    config.demag_compensation = BLHELI_S_DEMAG_LOW;
    config.temperature_protection = 0u;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.temperature_protection_limit == 0u);
    config.temperature_protection = 1u;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.temperature_protection_limit == 49u);
    config.temperature_protection = 7u;
    assert(blheli_s_decode_config(&config, &decoded));
    assert(decoded.temperature_protection_limit == 103u);
    config.temperature_protection = 8u;
    assert(!blheli_s_decode_config(&config, &decoded));
}
