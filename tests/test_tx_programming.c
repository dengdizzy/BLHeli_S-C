#include <assert.h>

#include "communication/tx_programming.h"

static void fill_samples(uint8_t *samples, uint8_t value)
{
    for (size_t i = 0u; i < BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES; ++i) {
        samples[i] = value;
    }
}

void test_tx_programming(void)
{
    uint8_t samples[BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES];
    uint8_t high_samples[BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES];
    uint8_t low_samples[BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES];
    uint8_t average;
    struct blheli_s_tx_programming_descriptor descriptor = {
        .dshot_input = false,
        .bidirectional = false,
        .tx_programming_enabled = true,
        .initial_arm = true,
        .new_rcp = BLHELI_S_TX_PROGRAMMING_MIDSTICK,
        .low_phase_new_rcp = 0u,
        .high_hold_count = BLHELI_S_TX_PROGRAMMING_HIGH_HOLD_COUNT,
        .low_hold_count = BLHELI_S_TX_PROGRAMMING_LOW_HOLD_COUNT,
        .min_throttle = 37u,
        .max_throttle = 208u,
        .high_samples = high_samples,
        .high_sample_count = BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES,
        .low_samples = low_samples,
        .low_sample_count = BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES
    };
    struct blheli_s_tx_programming_result result;

    fill_samples(samples, 128u);
    assert(blheli_s_tx_programming_average_throttle(
        samples, BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES, &average));
    assert(average == 128u);
    fill_samples(samples, 255u);
    assert(blheli_s_tx_programming_average_throttle(
        samples, BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES, &average));
    assert(average == 255u);
    assert(!blheli_s_tx_programming_average_throttle(
        samples, BLHELI_S_TX_PROGRAMMING_AVERAGE_SAMPLES - 1u, &average));

    descriptor.dshot_input = true;
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.result == BLHELI_S_TX_PROGRAMMING_SKIPPED);
    assert(result.skip_reason == BLHELI_S_TX_PROGRAMMING_SKIP_DSHOT);
    assert(result.trace.action_count == 0u);

    descriptor.dshot_input = false;
    descriptor.bidirectional = true;
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.skip_reason == BLHELI_S_TX_PROGRAMMING_SKIP_BIDIRECTIONAL);

    descriptor.bidirectional = false;
    descriptor.tx_programming_enabled = false;
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.skip_reason == BLHELI_S_TX_PROGRAMMING_SKIP_DISABLED);
    assert(result.flash_keys_invalid_intent);
    assert(result.trace.actions[0] == BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_3MS);

    descriptor.tx_programming_enabled = true;
    descriptor.initial_arm = false;
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.skip_reason == BLHELI_S_TX_PROGRAMMING_SKIP_NOT_INITIAL_ARM);

    descriptor.initial_arm = true;
    descriptor.new_rcp = BLHELI_S_TX_PROGRAMMING_MIDSTICK - 1u;
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.skip_reason == BLHELI_S_TX_PROGRAMMING_SKIP_BELOW_MIDSTICK);
    assert(result.enters_programming);
    assert(result.flash_keys_valid_intent);
    assert(result.flash_keys_invalid_intent);

    descriptor.new_rcp = BLHELI_S_TX_PROGRAMMING_MIDSTICK;
    descriptor.high_hold_count = 1u;
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.result == BLHELI_S_TX_PROGRAMMING_HIGH_HOLD);
    assert(result.high_throttle_detected);
    assert(result.beep_intent);

    fill_samples(high_samples, 180u);
    fill_samples(low_samples, 60u);
    descriptor.high_hold_count = BLHELI_S_TX_PROGRAMMING_HIGH_HOLD_COUNT;
    descriptor.low_phase_new_rcp = BLHELI_S_TX_PROGRAMMING_MIDSTICK;
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.result == BLHELI_S_TX_PROGRAMMING_WAIT_LOW_THROTTLE);
    assert(result.stores_max_throttle);
    assert(result.max_throttle == 180u);
    assert(result.success_beep_intent);

    descriptor.low_phase_new_rcp = BLHELI_S_TX_PROGRAMMING_MIDSTICK - 1u;
    descriptor.low_hold_count = 9u;
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.result == BLHELI_S_TX_PROGRAMMING_LOW_HOLD);
    assert(result.low_throttle_detected);

    descriptor.low_hold_count = BLHELI_S_TX_PROGRAMMING_LOW_HOLD_COUNT;
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.result == BLHELI_S_TX_PROGRAMMING_COMPLETE);
    assert(result.stores_max_throttle);
    assert(result.stores_min_throttle);
    assert(result.min_throttle == 63u);
    assert(result.max_throttle == 180u);
    assert(!result.adjusts_max_throttle);
    assert(result.save_eeprom_intent);
    assert(result.flash_keys_valid_intent);
    assert(result.flash_keys_invalid_intent);
    assert(result.trace.actions[0] == BLHELI_S_TX_PROGRAMMING_ACTION_WAIT_3MS);
    assert(result.trace.actions[1] ==
           BLHELI_S_TX_PROGRAMMING_ACTION_SET_FLASH_KEYS_VALID);
    assert(result.trace.actions[result.trace.action_count - 1u] ==
           BLHELI_S_TX_PROGRAMMING_ACTION_RETURN_TO_NO_SIGNAL);

    fill_samples(high_samples, 80u);
    fill_samples(low_samples, 60u);
    blheli_s_tx_programming_evaluate(&descriptor, &result);
    assert(result.result == BLHELI_S_TX_PROGRAMMING_COMPLETE);
    assert(result.min_throttle == 63u);
    assert(result.max_throttle == 98u);
    assert(result.adjusts_max_throttle);
}
