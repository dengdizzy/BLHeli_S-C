#include "communication/tx_programming.h"

static void append_tx_programming_action(
    struct blheli_s_tx_programming_trace *trace,
    enum blheli_s_tx_programming_action_kind action)
{
    if (trace->action_count < BLHELI_S_TX_PROGRAMMING_MAX_ACTIONS) {
        trace->actions[trace->action_count] = action;
        trace->action_count++;
    }
}

static void append_gain_window(struct blheli_s_tx_programming_result *result)
{
    result->full_range_gain_intent = true;
    result->find_throttle_gains_intent = true;
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_FULL_RANGE_ON);
    append_tx_programming_action(
        &result->trace,
        BLHELI_S_TX_PROGRAMMING_ACTION_FIND_THROTTLE_GAINS);
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_100MS);
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_FULL_RANGE_OFF);
    append_tx_programming_action(
        &result->trace,
        BLHELI_S_TX_PROGRAMMING_ACTION_FIND_THROTTLE_GAINS);
}

bool blheli_s_tx_programming_average_throttle(const uint8_t *samples,
                                              size_t sample_count,
                                              uint8_t *average)
{
    uint16_t sum = 0u;

    if (samples == 0 || average == 0 ||
        sample_count != BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES) {
        return false;
    }

    for (size_t i = 0u; i < sample_count; ++i) {
        sum = (uint16_t)(sum + samples[i]);
    }
    sum >>= 4u;
    *average = sum > UINT8_MAX ? UINT8_MAX : (uint8_t)sum;
    return true;
}

void blheli_s_tx_programming_evaluate(
    const struct blheli_s_tx_programming_descriptor *descriptor,
    struct blheli_s_tx_programming_result *result)
{
    uint8_t average;
    uint8_t minimum;

    *result = (struct blheli_s_tx_programming_result){
        .result = BLHELI_S_TX_PROGRAMMING_SKIPPED,
        .skip_reason = BLHELI_S_TX_PROGRAMMING_SKIP_NONE,
        .min_throttle = descriptor->min_throttle,
        .max_throttle = descriptor->max_throttle
    };

    if (descriptor->dshot_input) {
        result->skip_reason = BLHELI_S_TX_PROGRAMMING_SKIP_DSHOT;
        return;
    }
    if (descriptor->bidirectional) {
        result->skip_reason = BLHELI_S_TX_PROGRAMMING_SKIP_BIDIRECTIONAL;
        return;
    }

    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_3MS);

    if (!descriptor->tx_programming_enabled) {
        result->skip_reason = BLHELI_S_TX_PROGRAMMING_SKIP_DISABLED;
        result->flash_keys_invalid_intent = true;
        append_tx_programming_action(
            &result->trace,
            BLHELI_S_TX_PROGRAMMING_ACTION_SET_FLASH_KEYS_INVALID);
        return;
    }
    if (!descriptor->initial_arm) {
        result->skip_reason = BLHELI_S_TX_PROGRAMMING_SKIP_NOT_INITIAL_ARM;
        result->flash_keys_invalid_intent = true;
        append_tx_programming_action(
            &result->trace,
            BLHELI_S_TX_PROGRAMMING_ACTION_SET_FLASH_KEYS_INVALID);
        return;
    }

    result->enters_programming = true;
    result->result = BLHELI_S_TX_PROGRAMMING_ENTERED;
    result->flash_keys_valid_intent = true;
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_SET_FLASH_KEYS_VALID);

    append_gain_window(result);
    if (descriptor->new_rcp < BLHELI_S_TX_PROGRAMMING_MIDSTICK) {
        result->result = BLHELI_S_TX_PROGRAMMING_SKIPPED;
        result->skip_reason = BLHELI_S_TX_PROGRAMMING_SKIP_BELOW_MIDSTICK;
        result->flash_keys_invalid_intent = true;
        append_tx_programming_action(
            &result->trace,
            BLHELI_S_TX_PROGRAMMING_ACTION_SET_FLASH_KEYS_INVALID);
        return;
    }

    result->high_throttle_detected = true;
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_1MS);
    result->beep_intent = true;
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_BEEP_F4);
    if (descriptor->high_hold_count < BLHELI_S_TX_PROGRAMMING_HIGH_HOLD_COUNT ||
        !blheli_s_tx_programming_average_throttle(
            descriptor->high_samples, descriptor->high_sample_count, &average)) {
        result->result = BLHELI_S_TX_PROGRAMMING_HIGH_HOLD;
        return;
    }

    result->max_throttle = average;
    result->stores_max_throttle = true;
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_AVERAGE_THROTTLE);
    append_tx_programming_action(
        &result->trace,
        BLHELI_S_TX_PROGRAMMING_ACTION_STORE_MAX_THROTTLE);
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_200MS);
    result->success_beep_intent = true;
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_SUCCESS_BEEP);

    append_gain_window(result);
    if (descriptor->low_phase_new_rcp >= BLHELI_S_TX_PROGRAMMING_MIDSTICK) {
        result->result = BLHELI_S_TX_PROGRAMMING_WAIT_LOW_THROTTLE;
        return;
    }

    result->low_throttle_detected = true;
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_1MS);
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_BEEP_F1);
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_10MS);
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_BEEP_F1);
    if (descriptor->low_hold_count < BLHELI_S_TX_PROGRAMMING_LOW_HOLD_COUNT ||
        !blheli_s_tx_programming_average_throttle(
            descriptor->low_samples, descriptor->low_sample_count, &average)) {
        result->result = BLHELI_S_TX_PROGRAMMING_LOW_HOLD;
        return;
    }

    minimum = (uint8_t)(average + 3u);
    result->min_throttle = minimum;
    result->stores_min_throttle = true;
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_AVERAGE_THROTTLE);
    append_tx_programming_action(
        &result->trace,
        BLHELI_S_TX_PROGRAMMING_ACTION_STORE_MIN_THROTTLE);
    if ((uint8_t)(result->max_throttle - BLHELI_S_TX_PROGRAMMING_MIN_GAP) <
        minimum || result->max_throttle < BLHELI_S_TX_PROGRAMMING_MIN_GAP) {
        result->max_throttle =
            (uint8_t)(minimum + BLHELI_S_TX_PROGRAMMING_MIN_GAP);
        result->adjusts_max_throttle = true;
        append_tx_programming_action(
            &result->trace,
            BLHELI_S_TX_PROGRAMMING_ACTION_ADJUST_MAX_THROTTLE);
    }

    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_200MS);
    result->save_eeprom_intent = true;
    append_tx_programming_action(
        &result->trace,
        BLHELI_S_TX_PROGRAMMING_ACTION_ERASE_AND_STORE_EEPROM);
    append_tx_programming_action(
        &result->trace,
        BLHELI_S_TX_PROGRAMMING_ACTION_SUCCESS_BEEP_INVERTED);
    append_tx_programming_action(&result->trace,
                                 BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_100MS);
    append_tx_programming_action(
        &result->trace,
        BLHELI_S_TX_PROGRAMMING_ACTION_FIND_THROTTLE_GAINS);
    result->flash_keys_invalid_intent = true;
    append_tx_programming_action(
        &result->trace,
        BLHELI_S_TX_PROGRAMMING_ACTION_SET_FLASH_KEYS_INVALID);
    append_tx_programming_action(
        &result->trace,
        BLHELI_S_TX_PROGRAMMING_ACTION_RETURN_TO_NO_SIGNAL);
    result->result = BLHELI_S_TX_PROGRAMMING_COMPLETE;
}
