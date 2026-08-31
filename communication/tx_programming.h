#ifndef BLHELI_S_TX_PROGRAMMING_H
#define BLHELI_S_TX_PROGRAMMING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES UINT8_C(16)
#define BLHELI_S_TX_PROGRAMMING_MIDSTICK UINT8_C(127)
#define BLHELI_S_TX_PROGRAMMING_HIGH_HOLD_COUNT UINT8_C(2)
#define BLHELI_S_TX_PROGRAMMING_LOW_HOLD_COUNT UINT8_C(10)
#define BLHELI_S_TX_PROGRAMMING_MIN_GAP UINT8_C(35)
#define BLHELI_S_TX_PROGRAMMING_MAX_ACTIONS UINT8_C(32)

enum blheli_s_tx_programming_result_kind {
    BLHELI_S_TX_PROGRAMMING_SKIPPED,
    BLHELI_S_TX_PROGRAMMING_ENTERED,
    BLHELI_S_TX_PROGRAMMING_HIGH_HOLD,
    BLHELI_S_TX_PROGRAMMING_WAIT_LOW_THROTTLE,
    BLHELI_S_TX_PROGRAMMING_LOW_HOLD,
    BLHELI_S_TX_PROGRAMMING_COMPLETE
};

enum blheli_s_tx_programming_skip_reason {
    BLHELI_S_TX_PROGRAMMING_SKIP_NONE,
    BLHELI_S_TX_PROGRAMMING_SKIP_DSHOT,
    BLHELI_S_TX_PROGRAMMING_SKIP_BIDIRECTIONAL,
    BLHELI_S_TX_PROGRAMMING_SKIP_DISABLED,
    BLHELI_S_TX_PROGRAMMING_SKIP_NOT_INITIAL_ARM,
    BLHELI_S_TX_PROGRAMMING_SKIP_BELOW_MIDSTICK
};

enum blheli_s_tx_programming_action_kind {
    BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_3MS,
    BLHELI_S_TX_PROGRAMMING_ACTION_SET_FLASH_KEYS_VALID,
    BLHELI_S_TX_PROGRAMMING_ACTION_SET_FLASH_KEYS_INVALID,
    BLHELI_S_TX_PROGRAMMING_ACTION_FULL_RANGE_ON,
    BLHELI_S_TX_PROGRAMMING_ACTION_FULL_RANGE_OFF,
    BLHELI_S_TX_PROGRAMMING_ACTION_FIND_THROTTLE_GAINS,
    BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_100MS,
    BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_200MS,
    BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_1MS,
    BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_10MS,
    BLHELI_S_TX_PROGRAMMING_ACTION_BEEP_F1,
    BLHELI_S_TX_PROGRAMMING_ACTION_BEEP_F4,
    BLHELI_S_TX_PROGRAMMING_ACTION_SUCCESS_BEEP,
    BLHELI_S_TX_PROGRAMMING_ACTION_SUCCESS_BEEP_INVERTED,
    BLHELI_S_TX_PROGRAMMING_ACTION_AVERAGE_THROTTLE,
    BLHELI_S_TX_PROGRAMMING_ACTION_STORE_MAX_THROTTLE,
    BLHELI_S_TX_PROGRAMMING_ACTION_STORE_MIN_THROTTLE,
    BLHELI_S_TX_PROGRAMMING_ACTION_ADJUST_MAX_THROTTLE,
    BLHELI_S_TX_PROGRAMMING_ACTION_ERASE_AND_STORE_EEPROM,
    BLHELI_S_TX_PROGRAMMING_ACTION_RETURN_TO_NO_SIGNAL
};

struct blheli_s_tx_programming_trace {
    uint8_t action_count;
    enum blheli_s_tx_programming_action_kind
        actions[BLHELI_S_TX_PROGRAMMING_MAX_ACTIONS];
};

struct blheli_s_tx_programming_descriptor {
    bool dshot_input;
    bool bidirectional;
    bool tx_programming_enabled;
    bool initial_arm;
    uint8_t new_rcp;
    uint8_t low_phase_new_rcp;
    uint8_t high_hold_count;
    uint8_t low_hold_count;
    uint8_t min_throttle;
    uint8_t max_throttle;
    const uint8_t *high_samples;
    size_t high_sample_count;
    const uint8_t *low_samples;
    size_t low_sample_count;
};

struct blheli_s_tx_programming_result {
    enum blheli_s_tx_programming_result_kind result;
    enum blheli_s_tx_programming_skip_reason skip_reason;
    uint8_t min_throttle;
    uint8_t max_throttle;
    bool enters_programming;
    bool high_throttle_detected;
    bool low_throttle_detected;
    bool stores_min_throttle;
    bool stores_max_throttle;
    bool adjusts_max_throttle;
    bool save_eeprom_intent;
    bool flash_keys_valid_intent;
    bool flash_keys_invalid_intent;
    bool full_range_gain_intent;
    bool find_throttle_gains_intent;
    bool beep_intent;
    bool success_beep_intent;
    struct blheli_s_tx_programming_trace trace;
};

bool blheli_s_tx_programming_average_throttle(const uint8_t *samples,
                                              size_t sample_count,
                                              uint8_t *average);
void blheli_s_tx_programming_evaluate(
    const struct blheli_s_tx_programming_descriptor *descriptor,
    struct blheli_s_tx_programming_result *result);

#endif
