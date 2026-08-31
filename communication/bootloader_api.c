#include "communication/bootloader_api.h"

static void append_boot_action(struct blheli_s_bootloader_trace *trace,
                               enum blheli_s_bootloader_action_kind action)
{
    if (trace->action_count < BLHELI_S_BOOT_MAX_ACTIONS) {
        trace->actions[trace->action_count] = action;
        trace->action_count++;
    }
}

const char *blheli_s_bootloader_signature(void)
{
    return "BLHeli";
}

uint16_t blheli_s_bootloader_start_address(bool high_boot_start_variant)
{
    return high_boot_start_variant ? BLHELI_S_BOOT_START_HIGH :
        BLHELI_S_BOOT_START_LOW;
}

void blheli_s_bootloader_entry_decide(
    const struct blheli_s_bootloader_entry_descriptor *descriptor,
    struct blheli_s_bootloader_result *result)
{
    *result = (struct blheli_s_bootloader_result){
        .boot_start =
            blheli_s_bootloader_start_address(
                descriptor->high_boot_start_variant),
        .status_code = BLHELI_S_BOOT_SUCCESS
    };
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_CHECK_RTX_HIGH);
    if (descriptor->rtx_high_for_boot_window) {
        result->result = BLHELI_S_BOOT_RESULT_JUMP_BOOTLOADER;
        result->reset_or_jump_intent = true;
        append_boot_action(&result->trace,
                           BLHELI_S_BOOT_ACTION_JUMP_BOOTLOADER);
        return;
    }
    result->result = BLHELI_S_BOOT_RESULT_CONTINUE_APPLICATION;
    append_boot_action(&result->trace,
                       BLHELI_S_BOOT_ACTION_CONTINUE_APPLICATION);
}

void blheli_s_bootloader_init_trace(
    const struct blheli_s_bootloader_init_descriptor *descriptor,
    struct blheli_s_bootloader_result *result)
{
    *result = (struct blheli_s_bootloader_result){
        .result = BLHELI_S_BOOT_RESULT_INITIALIZED,
        .status_code = BLHELI_S_BOOT_SUCCESS
    };
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_DISABLE_INTERRUPTS);
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_DISABLE_WATCHDOG);
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_INIT_STACK);
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_INIT_CLOCK);
    if (descriptor->enable_vdd_monitor) {
        append_boot_action(&result->trace,
                           BLHELI_S_BOOT_ACTION_ENABLE_VDD_MONITOR);
    }
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_SET_FLASH_KEYS_VALID);
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_INIT_RTX_PIN);
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_ENABLE_CROSSBAR);
}

void blheli_s_bootloader_scan_signature(
    const struct blheli_s_bootloader_signature_descriptor *descriptor,
    struct blheli_s_bootloader_result *result)
{
    const char *signature = blheli_s_bootloader_signature();
    bool matches = descriptor->size == BLHELI_S_BOOT_SIGN_LENGTH;

    *result = (struct blheli_s_bootloader_result){
        .status_code = BLHELI_S_BOOT_SUCCESS
    };
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_SCAN_SIGNATURE);

    if (descriptor->timed_out || descriptor->bytes == 0) {
        result->result = BLHELI_S_BOOT_RESULT_SIGNATURE_REJECTED;
        result->status_code = BLHELI_S_BOOT_ERROR_COMMAND;
        return;
    }

    for (size_t i = 0u; i < descriptor->size &&
         i < BLHELI_S_BOOT_SIGN_LENGTH; ++i) {
        if (descriptor->bytes[i] != (uint8_t)signature[i]) {
            matches = false;
        }
    }

    if (!matches) {
        result->result = BLHELI_S_BOOT_RESULT_SIGNATURE_REJECTED;
        result->status_code = BLHELI_S_BOOT_ERROR_COMMAND;
        return;
    }
    if (!descriptor->crc_ok) {
        result->result = BLHELI_S_BOOT_RESULT_SIGNATURE_REJECTED;
        result->status_code = BLHELI_S_BOOT_ERROR_CRC;
        return;
    }

    result->result = BLHELI_S_BOOT_RESULT_SIGNATURE_ACCEPTED;
    result->boot_info_response_intent = true;
    append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_SEND_BOOT_INFO);
}

void blheli_s_bootloader_decode_command(
    const struct blheli_s_bootloader_command_descriptor *descriptor,
    struct blheli_s_bootloader_result *result)
{
    *result = (struct blheli_s_bootloader_result){
        .result = BLHELI_S_BOOT_RESULT_COMMAND,
        .command = BLHELI_S_BOOT_COMMAND_UNKNOWN,
        .status_code = BLHELI_S_BOOT_SUCCESS,
        .boot_start = descriptor->boot_start,
        .address = descriptor->current_address
    };

    if (!descriptor->crc_ok) {
        result->status_code = BLHELI_S_BOOT_ERROR_CRC;
        return;
    }

    if (descriptor->command_high == 0xffu) {
        result->command = BLHELI_S_BOOT_COMMAND_SET_ADDRESS;
        result->address = descriptor->argument;
        append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_SET_ADDRESS);
        return;
    }
    if (descriptor->command_high == 0xfeu) {
        result->command = BLHELI_S_BOOT_COMMAND_SET_BUFFER;
        result->buffer_length = descriptor->argument;
        append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_SET_BUFFER);
        return;
    }
    if (descriptor->command_high == 0xfdu) {
        result->command = BLHELI_S_BOOT_COMMAND_KEEP_ALIVE;
        append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_KEEP_ALIVE);
        return;
    }
    if (descriptor->command_high == 0u) {
        result->reset_or_jump_intent = true;
        if (descriptor->command_low == 0u) {
            result->command = BLHELI_S_BOOT_COMMAND_RESTART_BOOTLOADER;
            append_boot_action(&result->trace,
                               BLHELI_S_BOOT_ACTION_RESTART_BOOTLOADER);
        } else {
            result->command = BLHELI_S_BOOT_COMMAND_RUN_APPLICATION;
            append_boot_action(&result->trace,
                               BLHELI_S_BOOT_ACTION_RUN_APPLICATION);
            append_boot_action(&result->trace,
                               BLHELI_S_BOOT_ACTION_SET_FLASH_KEYS_INVALID);
        }
        return;
    }
    if (descriptor->command_high == 1u || descriptor->command_high == 2u) {
        bool program = descriptor->command_high == 1u;

        result->command = program ? BLHELI_S_BOOT_COMMAND_PROGRAM_FLASH :
            BLHELI_S_BOOT_COMMAND_ERASE_FLASH;
        if (descriptor->current_address >= descriptor->boot_start) {
            result->status_code = BLHELI_S_BOOT_ERROR_PROG;
            result->boot_area_rejected = true;
            append_boot_action(&result->trace,
                               BLHELI_S_BOOT_ACTION_REJECT_BOOT_AREA);
            return;
        }
        result->flash_operation_intent = true;
        append_boot_action(&result->trace, program ?
            BLHELI_S_BOOT_ACTION_PROGRAM_FLASH :
            BLHELI_S_BOOT_ACTION_ERASE_FLASH);
        return;
    }
    if (descriptor->command_high == 3u) {
        result->command = BLHELI_S_BOOT_COMMAND_READ_FLASH;
        result->reads_flash_intent = true;
        append_boot_action(&result->trace, BLHELI_S_BOOT_ACTION_READ_FLASH);
        return;
    }

    result->status_code = BLHELI_S_BOOT_ERROR_COMMAND;
}
