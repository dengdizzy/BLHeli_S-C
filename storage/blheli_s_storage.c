#include "storage/blheli_s_storage.h"

bool blheli_s_storage_header_valid(
    const uint8_t *record, size_t size)
{
    return size >= BLHELI_S_PARAMETER_RECORD_SIZE &&
           record[BLHELI_S_PARAMETER_OFFSET_LAYOUT_REVISION] ==
               BLHELI_S_PARAMETER_LAYOUT_REVISION &&
           record[BLHELI_S_PARAMETER_OFFSET_SIGNATURE_LOW] ==
               BLHELI_S_PARAMETER_SIGNATURE_LOW &&
           record[BLHELI_S_PARAMETER_OFFSET_SIGNATURE_HIGH] ==
               BLHELI_S_PARAMETER_SIGNATURE_HIGH;
}

bool blheli_s_storage_encode(
    const struct blheli_s_config *config, uint8_t *record, size_t size)
{
    if (size < BLHELI_S_PARAMETER_RECORD_SIZE) {
        return false;
    }
    for (size_t i = 0u; i < BLHELI_S_PARAMETER_RECORD_SIZE; ++i) {
        record[i] = BLHELI_S_PARAMETER_PLACEHOLDER;
    }
    record[BLHELI_S_PARAMETER_OFFSET_FW_MAIN] = 16u;
    record[BLHELI_S_PARAMETER_OFFSET_FW_SUB] = 7u;
    record[BLHELI_S_PARAMETER_OFFSET_LAYOUT_REVISION] =
        BLHELI_S_PARAMETER_LAYOUT_REVISION;
    record[BLHELI_S_PARAMETER_OFFSET_STARTUP_POWER] = config->startup_power;
    record[BLHELI_S_PARAMETER_OFFSET_DIRECTION] = config->direction;
    record[BLHELI_S_PARAMETER_OFFSET_SIGNATURE_LOW] =
        BLHELI_S_PARAMETER_SIGNATURE_LOW;
    record[BLHELI_S_PARAMETER_OFFSET_SIGNATURE_HIGH] =
        BLHELI_S_PARAMETER_SIGNATURE_HIGH;
    record[BLHELI_S_PARAMETER_OFFSET_ENABLE_TX_PROGRAMMING] =
        config->enable_tx_programming;
    record[BLHELI_S_PARAMETER_OFFSET_COMMUTATION_TIMING] =
        config->commutation_timing;
    record[BLHELI_S_PARAMETER_OFFSET_MIN_THROTTLE] = config->min_throttle;
    record[BLHELI_S_PARAMETER_OFFSET_MAX_THROTTLE] = config->max_throttle;
    record[BLHELI_S_PARAMETER_OFFSET_BEEP_STRENGTH] = config->beep_strength;
    record[BLHELI_S_PARAMETER_OFFSET_BEACON_STRENGTH] =
        config->beacon_strength;
    record[BLHELI_S_PARAMETER_OFFSET_BEACON_DELAY] = config->beacon_delay;
    record[BLHELI_S_PARAMETER_OFFSET_DEMAG_COMPENSATION] =
        config->demag_compensation;
    record[BLHELI_S_PARAMETER_OFFSET_CENTER_THROTTLE] =
        config->center_throttle;
    record[BLHELI_S_PARAMETER_OFFSET_TEMPERATURE_PROTECTION] =
        config->temperature_protection;
    record[BLHELI_S_PARAMETER_OFFSET_POWER_PROTECTION] =
        config->power_protection;
    record[BLHELI_S_PARAMETER_OFFSET_BRAKE_ON_STOP] = config->brake_on_stop;
    record[BLHELI_S_PARAMETER_OFFSET_LED_CONTROL] = config->led_control;
    for (size_t i = 0u; i < BLHELI_S_PARAMETER_NAME_LENGTH; ++i) {
        record[BLHELI_S_PARAMETER_NAME_OFFSET + i] = ' ';
    }
    return true;
}

bool blheli_s_storage_decode(
    const uint8_t *record, size_t size, struct blheli_s_config *config)
{
    if (!blheli_s_storage_header_valid(record, size)) {
        return false;
    }
    *config = (struct blheli_s_config){
        record[BLHELI_S_PARAMETER_OFFSET_STARTUP_POWER],
        record[BLHELI_S_PARAMETER_OFFSET_DIRECTION],
        record[BLHELI_S_PARAMETER_OFFSET_COMMUTATION_TIMING],
        record[BLHELI_S_PARAMETER_OFFSET_MIN_THROTTLE],
        record[BLHELI_S_PARAMETER_OFFSET_MAX_THROTTLE],
        record[BLHELI_S_PARAMETER_OFFSET_CENTER_THROTTLE],
        record[BLHELI_S_PARAMETER_OFFSET_BEEP_STRENGTH],
        record[BLHELI_S_PARAMETER_OFFSET_BEACON_STRENGTH],
        record[BLHELI_S_PARAMETER_OFFSET_BEACON_DELAY],
        record[BLHELI_S_PARAMETER_OFFSET_DEMAG_COMPENSATION],
        record[BLHELI_S_PARAMETER_OFFSET_TEMPERATURE_PROTECTION],
        record[BLHELI_S_PARAMETER_OFFSET_POWER_PROTECTION],
        record[BLHELI_S_PARAMETER_OFFSET_BRAKE_ON_STOP],
        record[BLHELI_S_PARAMETER_OFFSET_LED_CONTROL],
        record[BLHELI_S_PARAMETER_OFFSET_ENABLE_TX_PROGRAMMING]
    };
    return true;
}
