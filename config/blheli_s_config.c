#include "config/blheli_s_config.h"

static const uint8_t startup_power_table[] = {
    0x04u, 0x06u, 0x08u, 0x0cu, 0x10u, 0x18u, 0x20u,
    0x30u, 0x40u, 0x60u, 0x80u, 0xa0u, 0xc0u
};

void blheli_s_config_defaults(struct blheli_s_config *config)
{
    *config = (struct blheli_s_config){
        .startup_power = 9u, .direction = BLHELI_S_DIRECTION_NORMAL,
        .commutation_timing = 3u, .min_throttle = 37u, .max_throttle = 208u,
        .center_throttle = 122u, .beep_strength = 40u,
        .beacon_strength = 80u, .beacon_delay = 4u, .demag_compensation = 2u,
        .temperature_protection = 7u, .power_protection = 1u,
        .brake_on_stop = 0u, .led_control = 0u, .enable_tx_programming = 1u
    };
}

bool blheli_s_decode_config(const struct blheli_s_config *config,
                            struct blheli_s_decoded_config *decoded)
{
    uint8_t temperature_limit;

    if (config->startup_power == 0u ||
        config->startup_power > sizeof(startup_power_table)) {
        return false;
    }

    decoded->bidirectional = config->direction >= BLHELI_S_DIRECTION_BIDIRECTIONAL;
    decoded->direction_reversed = (config->direction & 2u) != 0u;
    decoded->bidirectional_reversed = decoded->direction_reversed;
    decoded->startup_power_decoded =
        startup_power_table[config->startup_power - 1u];
    decoded->low_rpm_power_slope = config->startup_power < 2u
        ? 2u : config->startup_power;
    decoded->demag_power_off_threshold = 255u;
    if (config->demag_compensation == BLHELI_S_DEMAG_LOW) {
        decoded->demag_power_off_threshold = 160u;
    } else if (config->demag_compensation == BLHELI_S_DEMAG_HIGH) {
        decoded->demag_power_off_threshold = 130u;
    }

    temperature_limit = 49u - 9u;
    for (uint8_t step = config->temperature_protection; step != 0u; --step) {
        temperature_limit = (uint8_t)(temperature_limit + 9u);
    }
    decoded->temperature_protection_limit = temperature_limit;
    return true;
}

uint16_t blheli_s_scale_throttle_calibration(uint8_t value, bool clock_48mhz)
{
    uint16_t scaled = 1024u + ((uint16_t)value << 2u) +
                      (((uint16_t)value * 12u) >> 8u);
    return clock_48mhz ? (uint16_t)(scaled << 1u) : scaled;
}
