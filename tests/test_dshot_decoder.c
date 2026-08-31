#include <assert.h>

#include "communication/dshot_commands.h"
#include "communication/dshot_decoder.h"

static uint16_t make_frame(uint16_t throttle, bool telemetry)
{
    uint16_t value = (uint16_t)((throttle << 1) | (telemetry ? 1u : 0u));
    uint8_t checksum = (uint8_t)((value ^ (value >> 4) ^
                                  (value >> 8)) & 0x0fu);
    return (uint16_t)((value << 4) | checksum);
}

void test_dshot_decoder(void)
{
    struct blheli_s_dshot_packet packet;
    struct blheli_s_dshot_command_state state = {
        .command = 0u,
        .repeat_count = 0u,
        .configured_direction = BLHELI_S_DIRECTION_NORMAL,
        .stored_direction = BLHELI_S_DIRECTION_REVERSED,
        .bidirectional = false,
        .direction_reversed = false,
        .bidirectional_reversed = false
    };
    struct blheli_s_dshot_command_result result;

    assert(blheli_s_dshot_decode(make_frame(1000u, false), &packet));
    assert(packet.throttle == 1000u);
    assert(!packet.is_command);
    assert(!packet.telemetry);
    assert(!packet.special_command_range);
    assert(blheli_s_dshot_decode(make_frame(12u, true), &packet));
    assert(packet.is_command);
    assert(packet.command == 6u);
    assert(packet.telemetry);
    assert(packet.special_command_range);
    assert(blheli_s_dshot_decode(make_frame(12u, false), &packet));
    assert(!packet.is_command);
    assert(packet.command == 0u);
    assert(packet.throttle == 0u);
    assert(packet.special_command_range);
    assert(!blheli_s_dshot_decode(
        (uint16_t)(make_frame(1000u, false) ^ 1u), &packet));

    blheli_s_dshot_command_latch(&state, &packet, &result);
    assert(result.result == BLHELI_S_DSHOT_COMMAND_CLEARED);
    assert(state.command == 0u);
    assert(state.repeat_count == 0u);
    assert(result.trace.actions[0] ==
           BLHELI_S_DSHOT_COMMAND_ACTION_CLEAR_COMMAND);

    assert(blheli_s_dshot_decode(make_frame(14u, true), &packet));
    blheli_s_dshot_command_latch(&state, &packet, &result);
    assert(result.result == BLHELI_S_DSHOT_COMMAND_PENDING);
    assert(state.command == 7u);
    assert(state.repeat_count == 0u);
    assert(result.trace.actions[0] ==
           BLHELI_S_DSHOT_COMMAND_ACTION_LATCH_COMMAND);
    blheli_s_dshot_command_latch(&state, &packet, &result);
    assert(state.command == 7u);
    assert(state.repeat_count == 1u);
    assert(result.trace.actions[0] ==
           BLHELI_S_DSHOT_COMMAND_ACTION_INCREMENT_REPEAT_COUNT);

    assert(blheli_s_dshot_decode(make_frame(16u, true), &packet));
    blheli_s_dshot_command_latch(&state, &packet, &result);
    assert(state.command == 8u);
    assert(state.repeat_count == 0u);

    state.command = 7u;
    state.repeat_count = 5u;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.result == BLHELI_S_DSHOT_COMMAND_PENDING);
    assert(!result.clears_command);

    state.repeat_count = BLHELI_S_DSHOT_COMMAND_REPEAT_THRESHOLD;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.result == BLHELI_S_DSHOT_COMMAND_SET_DIRECTION);
    assert(result.configured_direction == BLHELI_S_DIRECTION_NORMAL);
    assert(!result.direction_reversed);
    assert(result.clears_command);
    assert(result.trace.actions[0] ==
           BLHELI_S_DSHOT_COMMAND_ACTION_SET_DIRECTION);

    state.command = 8u;
    state.bidirectional = false;
    state.configured_direction = BLHELI_S_DIRECTION_NORMAL;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.configured_direction == BLHELI_S_DIRECTION_REVERSED);
    assert(result.direction_reversed);

    state.command = 9u;
    state.bidirectional = true;
    state.configured_direction = BLHELI_S_DIRECTION_BIDIRECTIONAL_REVERSED;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.configured_direction == BLHELI_S_DIRECTION_REVERSED);
    assert(!result.bidirectional);

    state.command = 10u;
    state.bidirectional = false;
    state.configured_direction = BLHELI_S_DIRECTION_REVERSED;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.configured_direction ==
           BLHELI_S_DIRECTION_BIDIRECTIONAL_REVERSED);
    assert(result.bidirectional);

    state.command = 20u;
    state.stored_direction = BLHELI_S_DIRECTION_BIDIRECTIONAL;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.configured_direction == BLHELI_S_DIRECTION_BIDIRECTIONAL);
    assert(result.bidirectional);
    assert(result.direction_reversed);

    state.command = 21u;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.configured_direction ==
           BLHELI_S_DIRECTION_BIDIRECTIONAL_REVERSED);
    assert(!result.direction_reversed);

    state.command = 1u;
    state.repeat_count = 0u;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.result == BLHELI_S_DSHOT_COMMAND_BEEP);
    assert(result.beep_frequency == 1u);
    assert(result.switch_power_off_intent);
    assert(result.load_beacon_strength_intent);
    assert(result.load_beep_strength_intent);
    assert(result.wait_100ms_intent);
    assert(result.clears_command);
    assert(result.trace.actions[0] ==
           BLHELI_S_DSHOT_COMMAND_ACTION_SWITCH_POWER_OFF);
    assert(result.trace.actions[2] == BLHELI_S_DSHOT_COMMAND_ACTION_BEEP);

    state.command = 5u;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.beep_frequency == 4u);

    state.command = 12u;
    state.repeat_count = 5u;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.result == BLHELI_S_DSHOT_COMMAND_PENDING);
    assert(result.flash_keys_valid_intent);
    assert(result.flash_keys_invalid_intent);
    assert(!result.save_settings_intent);
    assert(!result.clears_command);

    state.repeat_count = BLHELI_S_DSHOT_COMMAND_REPEAT_THRESHOLD;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.result == BLHELI_S_DSHOT_COMMAND_SAVE_SETTINGS);
    assert(result.save_settings_intent);
    assert(result.clears_command);
    assert(result.flash_keys_valid_intent);
    assert(result.flash_keys_invalid_intent);

    state.command = 13u;
    blheli_s_dshot_command_execute(&state, &result);
    assert(result.result == BLHELI_S_DSHOT_COMMAND_CLEARED);
    assert(result.clears_command);
}
