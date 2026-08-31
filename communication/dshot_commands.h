#ifndef BLHELI_S_DSHOT_COMMANDS_H
#define BLHELI_S_DSHOT_COMMANDS_H

#include <stdbool.h>
#include <stdint.h>

#include "communication/dshot_decoder.h"
#include "config/blheli_s_config.h"

#define BLHELI_S_DSHOT_COMMAND_REPEAT_THRESHOLD UINT8_C(6)
#define BLHELI_S_DSHOT_COMMAND_MAX_ACTIONS UINT8_C(8)

enum blheli_s_dshot_command_action_kind {
    BLHELI_S_DSHOT_COMMAND_ACTION_CLEAR_COMMAND,
    BLHELI_S_DSHOT_COMMAND_ACTION_LATCH_COMMAND,
    BLHELI_S_DSHOT_COMMAND_ACTION_INCREMENT_REPEAT_COUNT,
    BLHELI_S_DSHOT_COMMAND_ACTION_SWITCH_POWER_OFF,
    BLHELI_S_DSHOT_COMMAND_ACTION_LOAD_BEACON_STRENGTH,
    BLHELI_S_DSHOT_COMMAND_ACTION_BEEP,
    BLHELI_S_DSHOT_COMMAND_ACTION_LOAD_BEEP_STRENGTH,
    BLHELI_S_DSHOT_COMMAND_ACTION_WAIT_100MS,
    BLHELI_S_DSHOT_COMMAND_ACTION_SET_DIRECTION,
    BLHELI_S_DSHOT_COMMAND_ACTION_SET_FLASH_KEYS_VALID,
    BLHELI_S_DSHOT_COMMAND_ACTION_SAVE_SETTINGS,
    BLHELI_S_DSHOT_COMMAND_ACTION_SET_FLASH_KEYS_INVALID
};

enum blheli_s_dshot_command_result_kind {
    BLHELI_S_DSHOT_COMMAND_NONE,
    BLHELI_S_DSHOT_COMMAND_PENDING,
    BLHELI_S_DSHOT_COMMAND_BEEP,
    BLHELI_S_DSHOT_COMMAND_SET_DIRECTION,
    BLHELI_S_DSHOT_COMMAND_SAVE_SETTINGS,
    BLHELI_S_DSHOT_COMMAND_CLEARED
};

struct blheli_s_dshot_command_trace {
    uint8_t action_count;
    enum blheli_s_dshot_command_action_kind
        actions[BLHELI_S_DSHOT_COMMAND_MAX_ACTIONS];
};

struct blheli_s_dshot_command_state {
    uint8_t command;
    uint8_t repeat_count;
    uint8_t configured_direction;
    uint8_t stored_direction;
    bool bidirectional;
    bool direction_reversed;
    bool bidirectional_reversed;
};

struct blheli_s_dshot_command_result {
    enum blheli_s_dshot_command_result_kind result;
    uint8_t command;
    uint8_t repeat_count;
    uint8_t beep_frequency;
    uint8_t configured_direction;
    bool bidirectional;
    bool direction_reversed;
    bool bidirectional_reversed;
    bool clears_command;
    bool switch_power_off_intent;
    bool load_beacon_strength_intent;
    bool load_beep_strength_intent;
    bool wait_100ms_intent;
    bool flash_keys_valid_intent;
    bool flash_keys_invalid_intent;
    bool save_settings_intent;
    struct blheli_s_dshot_command_trace trace;
};

void blheli_s_dshot_command_latch(
    struct blheli_s_dshot_command_state *state,
    const struct blheli_s_dshot_packet *packet,
    struct blheli_s_dshot_command_result *result);
void blheli_s_dshot_command_execute(
    const struct blheli_s_dshot_command_state *state,
    struct blheli_s_dshot_command_result *result);

#endif
