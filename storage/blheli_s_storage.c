#include "storage/blheli_s_storage.h"

#include "storage/parameter_layout.h"

bool blheli_s_storage_header_valid(
    const uint8_t *record, size_t size)
{
    return size >= BLHELI_S_PARAMETER_RECORD_SIZE &&
           record[2] == BLHELI_S_PARAMETER_LAYOUT_REVISION &&
           record[13] == BLHELI_S_PARAMETER_SIGNATURE_LOW &&
           record[14] == BLHELI_S_PARAMETER_SIGNATURE_HIGH;
}

bool blheli_s_storage_encode(
    const struct blheli_s_config *config, uint8_t *record, size_t size)
{
    if (size < BLHELI_S_PARAMETER_RECORD_SIZE) {
        return false;
    }
    record[0] = 16u;
    record[1] = 7u;
    record[2] = BLHELI_S_PARAMETER_LAYOUT_REVISION;
    record[3] = record[4] = record[5] = record[6] = record[7] = 0xffu;
    record[8] = config->startup_power;
    record[9] = 0xffu;
    record[10] = config->direction;
    record[11] = record[12] = 0xffu;
    record[13] = BLHELI_S_PARAMETER_SIGNATURE_LOW;
    record[14] = BLHELI_S_PARAMETER_SIGNATURE_HIGH;
    record[15] = config->enable_tx_programming;
    record[16] = record[17] = record[18] = record[19] = record[20] = 0xffu;
    record[21] = config->commutation_timing;
    record[22] = record[23] = record[24] = 0xffu;
    record[25] = config->min_throttle;
    record[26] = config->max_throttle;
    record[27] = config->beep_strength;
    record[28] = config->beacon_strength;
    record[29] = config->beacon_delay;
    record[30] = 0xffu;
    record[31] = config->demag_compensation;
    record[32] = 0xffu;
    record[33] = config->center_throttle;
    record[34] = 0xffu;
    record[35] = config->temperature_protection;
    record[36] = config->power_protection;
    record[37] = record[38] = 0xffu;
    record[39] = config->brake_on_stop;
    record[40] = config->led_control;
    record[41] = 0xffu;
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
        record[8], record[10], record[21], record[25], record[26],
        record[33], record[27], record[28], record[29], record[31],
        record[35], record[36], record[39], record[40], record[15]
    };
    return true;
}
