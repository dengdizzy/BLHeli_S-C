#include "communication/dshot_commands.h"

static void append_dshot_command_action(
    struct blheli_s_dshot_command_trace *trace,
    enum blheli_s_dshot_command_action_kind action)
{
    if (trace->action_count < BLHELI_S_DSHOT_COMMAND_MAX_ACTIONS) {
        trace->actions[trace->action_count] = action;
        trace->action_count++;
    }
}

static void clear_command(struct blheli_s_dshot_command_result *result)
{
    result->command = 0u;
    result->repeat_count = 0u;
    result->clears_command = true;
    append_dshot_command_action(&result->trace,
                                BLHELI_S_DSHOT_COMMAND_ACTION_CLEAR_COMMAND);
}

static void set_direction_flags(struct blheli_s_dshot_command_result *result,
                                uint8_t direction)
{
    result->configured_direction = direction;
    result->bidirectional = direction >= BLHELI_S_DIRECTION_BIDIRECTIONAL;
    result->direction_reversed = (direction & 2u) != 0u;
    result->bidirectional_reversed = result->direction_reversed;
}

static uint8_t temporary_reverse_direction(uint8_t stored_direction)
{
    if (stored_direction == BLHELI_S_DIRECTION_NORMAL) {
        return BLHELI_S_DIRECTION_REVERSED;
    }
    if (stored_direction == BLHELI_S_DIRECTION_REVERSED) {
        return BLHELI_S_DIRECTION_NORMAL;
    }
    if (stored_direction == BLHELI_S_DIRECTION_BIDIRECTIONAL) {
        return BLHELI_S_DIRECTION_BIDIRECTIONAL_REVERSED;
    }
    if (stored_direction == BLHELI_S_DIRECTION_BIDIRECTIONAL_REVERSED) {
        return BLHELI_S_DIRECTION_BIDIRECTIONAL;
    }
    return stored_direction;
}

void blheli_s_dshot_command_latch(
    struct blheli_s_dshot_command_state *state,
    const struct blheli_s_dshot_packet *packet,
    struct blheli_s_dshot_command_result *result)
{
    *result = (struct blheli_s_dshot_command_result){
        .result = BLHELI_S_DSHOT_COMMAND_NONE,
        .command = state->command,
        .repeat_count = state->repeat_count,
        .configured_direction = state->configured_direction,
        .bidirectional = state->bidirectional,
        .direction_reversed = state->direction_reversed,
        .bidirectional_reversed = state->bidirectional_reversed
    };

    if (!packet->is_command) {
        if (packet->special_command_range || packet->throttle != 0u) {
            state->command = 0u;
            state->repeat_count = 0u;
            result->result = BLHELI_S_DSHOT_COMMAND_CLEARED;
            clear_command(result);
        }
        return;
    }

    if (packet->command == state->command) {
        state->repeat_count = (uint8_t)(state->repeat_count + 1u);
        result->repeat_count = state->repeat_count;
        result->result = BLHELI_S_DSHOT_COMMAND_PENDING;
        append_dshot_command_action(
            &result->trace,
            BLHELI_S_DSHOT_COMMAND_ACTION_INCREMENT_REPEAT_COUNT);
        return;
    }

    state->command = packet->command;
    state->repeat_count = 0u;
    result->command = state->command;
    result->repeat_count = state->repeat_count;
    result->result = BLHELI_S_DSHOT_COMMAND_PENDING;
    append_dshot_command_action(&result->trace,
                                BLHELI_S_DSHOT_COMMAND_ACTION_LATCH_COMMAND);
}

void blheli_s_dshot_command_execute(
    const struct blheli_s_dshot_command_state *state,
    struct blheli_s_dshot_command_result *result)
{
    uint8_t direction;

    *result = (struct blheli_s_dshot_command_result){
        .result = BLHELI_S_DSHOT_COMMAND_NONE,
        .command = state->command,
        .repeat_count = state->repeat_count,
        .configured_direction = state->configured_direction,
        .bidirectional = state->bidirectional,
        .direction_reversed = state->direction_reversed,
        .bidirectional_reversed = state->bidirectional_reversed
    };

    if (state->command == 0u) {
        return;
    }

    if (state->command >= 1u && state->command <= 5u) {
        result->result = BLHELI_S_DSHOT_COMMAND_BEEP;
        result->beep_frequency = state->command == 5u ? 4u : state->command;
        result->switch_power_off_intent = true;
        result->load_beacon_strength_intent = true;
        result->load_beep_strength_intent = true;
        result->wait_100ms_intent = true;
        append_dshot_command_action(
            &result->trace, BLHELI_S_DSHOT_COMMAND_ACTION_SWITCH_POWER_OFF);
        append_dshot_command_action(
            &result->trace,
            BLHELI_S_DSHOT_COMMAND_ACTION_LOAD_BEACON_STRENGTH);
        append_dshot_command_action(&result->trace,
                                    BLHELI_S_DSHOT_COMMAND_ACTION_BEEP);
        append_dshot_command_action(
            &result->trace, BLHELI_S_DSHOT_COMMAND_ACTION_LOAD_BEEP_STRENGTH);
        append_dshot_command_action(&result->trace,
                                    BLHELI_S_DSHOT_COMMAND_ACTION_WAIT_100MS);
        clear_command(result);
        return;
    }

    if (state->command == 12u) {
        result->flash_keys_valid_intent = true;
        append_dshot_command_action(
            &result->trace,
            BLHELI_S_DSHOT_COMMAND_ACTION_SET_FLASH_KEYS_VALID);
        if (state->repeat_count < BLHELI_S_DSHOT_COMMAND_REPEAT_THRESHOLD) {
            result->result = BLHELI_S_DSHOT_COMMAND_PENDING;
            result->flash_keys_invalid_intent = true;
            append_dshot_command_action(
                &result->trace,
                BLHELI_S_DSHOT_COMMAND_ACTION_SET_FLASH_KEYS_INVALID);
            return;
        }
        result->result = BLHELI_S_DSHOT_COMMAND_SAVE_SETTINGS;
        result->save_settings_intent = true;
        append_dshot_command_action(
            &result->trace, BLHELI_S_DSHOT_COMMAND_ACTION_SAVE_SETTINGS);
        clear_command(result);
        result->flash_keys_invalid_intent = true;
        append_dshot_command_action(
            &result->trace,
            BLHELI_S_DSHOT_COMMAND_ACTION_SET_FLASH_KEYS_INVALID);
        return;
    }

    if (state->command == 7u || state->command == 8u ||
        state->command == 9u || state->command == 10u ||
        state->command == 20u || state->command == 21u) {
        if (state->repeat_count < BLHELI_S_DSHOT_COMMAND_REPEAT_THRESHOLD) {
            result->result = BLHELI_S_DSHOT_COMMAND_PENDING;
            return;
        }

        direction = state->configured_direction;
        if (state->command == 7u) {
            direction = state->bidirectional ?
                BLHELI_S_DIRECTION_BIDIRECTIONAL : BLHELI_S_DIRECTION_NORMAL;
        } else if (state->command == 8u) {
            direction = state->bidirectional ?
                BLHELI_S_DIRECTION_BIDIRECTIONAL_REVERSED :
                BLHELI_S_DIRECTION_REVERSED;
        } else if (state->command == 9u) {
            if (!state->bidirectional) {
                result->result = BLHELI_S_DSHOT_COMMAND_CLEARED;
                clear_command(result);
                return;
            }
            direction = (uint8_t)(state->configured_direction - 2u);
        } else if (state->command == 10u) {
            if (state->bidirectional) {
                result->result = BLHELI_S_DSHOT_COMMAND_CLEARED;
                clear_command(result);
                return;
            }
            direction = (uint8_t)(state->configured_direction + 2u);
        } else if (state->command == 20u) {
            direction = state->stored_direction;
        } else {
            direction = temporary_reverse_direction(state->stored_direction);
        }

        result->result = BLHELI_S_DSHOT_COMMAND_SET_DIRECTION;
        set_direction_flags(result, direction);
        append_dshot_command_action(
            &result->trace, BLHELI_S_DSHOT_COMMAND_ACTION_SET_DIRECTION);
        clear_command(result);
        return;
    }

    result->result = BLHELI_S_DSHOT_COMMAND_CLEARED;
    clear_command(result);
}
