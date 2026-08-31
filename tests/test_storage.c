#include <assert.h>
#include <string.h>

#include "storage/blheli_s_storage.h"

static bool is_modeled_or_named_byte(size_t offset)
{
    switch (offset) {
    case BLHELI_S_PARAMETER_OFFSET_FW_MAIN:
    case BLHELI_S_PARAMETER_OFFSET_FW_SUB:
    case BLHELI_S_PARAMETER_OFFSET_LAYOUT_REVISION:
    case BLHELI_S_PARAMETER_OFFSET_STARTUP_POWER:
    case BLHELI_S_PARAMETER_OFFSET_DIRECTION:
    case BLHELI_S_PARAMETER_OFFSET_SIGNATURE_LOW:
    case BLHELI_S_PARAMETER_OFFSET_SIGNATURE_HIGH:
    case BLHELI_S_PARAMETER_OFFSET_ENABLE_TX_PROGRAMMING:
    case BLHELI_S_PARAMETER_OFFSET_COMMUTATION_TIMING:
    case BLHELI_S_PARAMETER_OFFSET_MIN_THROTTLE:
    case BLHELI_S_PARAMETER_OFFSET_MAX_THROTTLE:
    case BLHELI_S_PARAMETER_OFFSET_BEEP_STRENGTH:
    case BLHELI_S_PARAMETER_OFFSET_BEACON_STRENGTH:
    case BLHELI_S_PARAMETER_OFFSET_BEACON_DELAY:
    case BLHELI_S_PARAMETER_OFFSET_DEMAG_COMPENSATION:
    case BLHELI_S_PARAMETER_OFFSET_CENTER_THROTTLE:
    case BLHELI_S_PARAMETER_OFFSET_TEMPERATURE_PROTECTION:
    case BLHELI_S_PARAMETER_OFFSET_POWER_PROTECTION:
    case BLHELI_S_PARAMETER_OFFSET_BRAKE_ON_STOP:
    case BLHELI_S_PARAMETER_OFFSET_LED_CONTROL:
        return true;
    default:
        return offset >= BLHELI_S_PARAMETER_NAME_OFFSET &&
               offset < (size_t)(BLHELI_S_PARAMETER_NAME_OFFSET +
                                  BLHELI_S_PARAMETER_NAME_LENGTH);
    }
}

void test_storage(void)
{
    struct blheli_s_config original;
    struct blheli_s_config decoded;
    uint8_t record[BLHELI_S_PARAMETER_RECORD_SIZE];

    blheli_s_config_defaults(&original);
    memset(record, 0u, sizeof(record));
    assert(blheli_s_storage_encode(&original, record, sizeof(record)));
    assert(record[BLHELI_S_PARAMETER_OFFSET_FW_MAIN] == 16u);
    assert(record[BLHELI_S_PARAMETER_OFFSET_FW_SUB] == 7u);
    assert(record[BLHELI_S_PARAMETER_OFFSET_LAYOUT_REVISION] ==
           BLHELI_S_PARAMETER_LAYOUT_REVISION);
    assert(record[BLHELI_S_PARAMETER_OFFSET_STARTUP_POWER] ==
           original.startup_power);
    assert(record[BLHELI_S_PARAMETER_OFFSET_DIRECTION] == original.direction);
    assert(record[BLHELI_S_PARAMETER_OFFSET_SIGNATURE_LOW] ==
           BLHELI_S_PARAMETER_SIGNATURE_LOW);
    assert(record[BLHELI_S_PARAMETER_OFFSET_SIGNATURE_HIGH] ==
           BLHELI_S_PARAMETER_SIGNATURE_HIGH);
    assert(record[BLHELI_S_PARAMETER_OFFSET_ENABLE_TX_PROGRAMMING] ==
           original.enable_tx_programming);
    assert(record[BLHELI_S_PARAMETER_OFFSET_COMMUTATION_TIMING] ==
           original.commutation_timing);
    assert(record[BLHELI_S_PARAMETER_OFFSET_MIN_THROTTLE] ==
           original.min_throttle);
    assert(record[BLHELI_S_PARAMETER_OFFSET_MAX_THROTTLE] ==
           original.max_throttle);
    assert(record[BLHELI_S_PARAMETER_OFFSET_BEEP_STRENGTH] ==
           original.beep_strength);
    assert(record[BLHELI_S_PARAMETER_OFFSET_BEACON_STRENGTH] ==
           original.beacon_strength);
    assert(record[BLHELI_S_PARAMETER_OFFSET_BEACON_DELAY] ==
           original.beacon_delay);
    assert(record[BLHELI_S_PARAMETER_OFFSET_DEMAG_COMPENSATION] ==
           original.demag_compensation);
    assert(record[BLHELI_S_PARAMETER_OFFSET_CENTER_THROTTLE] ==
           original.center_throttle);
    assert(record[BLHELI_S_PARAMETER_OFFSET_TEMPERATURE_PROTECTION] ==
           original.temperature_protection);
    assert(record[BLHELI_S_PARAMETER_OFFSET_POWER_PROTECTION] ==
           original.power_protection);
    assert(record[BLHELI_S_PARAMETER_OFFSET_BRAKE_ON_STOP] ==
           original.brake_on_stop);
    assert(record[BLHELI_S_PARAMETER_OFFSET_LED_CONTROL] ==
           original.led_control);
    for (size_t i = 0u; i < BLHELI_S_PARAMETER_RECORD_SIZE; ++i) {
        if (!is_modeled_or_named_byte(i)) {
            assert(record[i] == BLHELI_S_PARAMETER_PLACEHOLDER);
        }
    }
    for (size_t i = 0u; i < BLHELI_S_PARAMETER_NAME_LENGTH; ++i) {
        assert(record[BLHELI_S_PARAMETER_NAME_OFFSET + i] == ' ');
    }
    assert(blheli_s_storage_header_valid(record, sizeof(record)));
    assert(blheli_s_storage_decode(record, sizeof(record), &decoded));
    assert(memcmp(&original, &decoded, sizeof(original)) == 0);
    assert(!blheli_s_storage_encode(&original, record,
                                    BLHELI_S_PARAMETER_RECORD_SIZE - 1u));
    assert(!blheli_s_storage_header_valid(record,
                                          BLHELI_S_PARAMETER_RECORD_SIZE - 1u));
    record[BLHELI_S_PARAMETER_OFFSET_SIGNATURE_LOW] = 0u;
    assert(!blheli_s_storage_header_valid(record, sizeof(record)));
    assert(!blheli_s_storage_decode(record, sizeof(record), &decoded));
}
