#ifndef BLHELI_S_BOOTLOADER_API_H
#define BLHELI_S_BOOTLOADER_API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BLHELI_S_BOOT_START_LOW UINT16_C(0x1c00)
#define BLHELI_S_BOOT_START_HIGH UINT16_C(0xf000)
#define BLHELI_S_BOOT_VERSION UINT8_C(6)
#define BLHELI_S_BOOT_PAGES UINT8_C(1)
#define BLHELI_S_BOOT_SUCCESS UINT8_C(0x30)
#define BLHELI_S_BOOT_ERROR_VERIFY UINT8_C(0xc0)
#define BLHELI_S_BOOT_ERROR_COMMAND UINT8_C(0xc1)
#define BLHELI_S_BOOT_ERROR_CRC UINT8_C(0xc2)
#define BLHELI_S_BOOT_ERROR_PROG UINT8_C(0xc5)
#define BLHELI_S_BOOT_CRC_POLYNOMIAL UINT16_C(0xa001)
#define BLHELI_S_BOOT_SIGN_LENGTH UINT8_C(6)
#define BLHELI_S_BOOT_MAX_ACTIONS UINT8_C(16)

enum blheli_s_bootloader_action_kind {
    BLHELI_S_BOOT_ACTION_CHECK_RTX_HIGH,
    BLHELI_S_BOOT_ACTION_JUMP_BOOTLOADER,
    BLHELI_S_BOOT_ACTION_CONTINUE_APPLICATION,
    BLHELI_S_BOOT_ACTION_DISABLE_INTERRUPTS,
    BLHELI_S_BOOT_ACTION_DISABLE_WATCHDOG,
    BLHELI_S_BOOT_ACTION_INIT_STACK,
    BLHELI_S_BOOT_ACTION_INIT_CLOCK,
    BLHELI_S_BOOT_ACTION_ENABLE_VDD_MONITOR,
    BLHELI_S_BOOT_ACTION_SET_FLASH_KEYS_VALID,
    BLHELI_S_BOOT_ACTION_SET_FLASH_KEYS_INVALID,
    BLHELI_S_BOOT_ACTION_INIT_RTX_PIN,
    BLHELI_S_BOOT_ACTION_ENABLE_CROSSBAR,
    BLHELI_S_BOOT_ACTION_SCAN_SIGNATURE,
    BLHELI_S_BOOT_ACTION_SEND_BOOT_INFO,
    BLHELI_S_BOOT_ACTION_SET_ADDRESS,
    BLHELI_S_BOOT_ACTION_SET_BUFFER,
    BLHELI_S_BOOT_ACTION_PROGRAM_FLASH,
    BLHELI_S_BOOT_ACTION_ERASE_FLASH,
    BLHELI_S_BOOT_ACTION_READ_FLASH,
    BLHELI_S_BOOT_ACTION_KEEP_ALIVE,
    BLHELI_S_BOOT_ACTION_RUN_APPLICATION,
    BLHELI_S_BOOT_ACTION_RESTART_BOOTLOADER,
    BLHELI_S_BOOT_ACTION_REJECT_BOOT_AREA
};

enum blheli_s_bootloader_result_kind {
    BLHELI_S_BOOT_RESULT_CONTINUE_APPLICATION,
    BLHELI_S_BOOT_RESULT_JUMP_BOOTLOADER,
    BLHELI_S_BOOT_RESULT_INITIALIZED,
    BLHELI_S_BOOT_RESULT_SIGNATURE_ACCEPTED,
    BLHELI_S_BOOT_RESULT_SIGNATURE_REJECTED,
    BLHELI_S_BOOT_RESULT_COMMAND
};

enum blheli_s_bootloader_command_kind {
    BLHELI_S_BOOT_COMMAND_RUN_APPLICATION,
    BLHELI_S_BOOT_COMMAND_RESTART_BOOTLOADER,
    BLHELI_S_BOOT_COMMAND_PROGRAM_FLASH,
    BLHELI_S_BOOT_COMMAND_ERASE_FLASH,
    BLHELI_S_BOOT_COMMAND_READ_FLASH,
    BLHELI_S_BOOT_COMMAND_SET_ADDRESS,
    BLHELI_S_BOOT_COMMAND_SET_BUFFER,
    BLHELI_S_BOOT_COMMAND_KEEP_ALIVE,
    BLHELI_S_BOOT_COMMAND_UNKNOWN
};

struct blheli_s_bootloader_trace {
    uint8_t action_count;
    enum blheli_s_bootloader_action_kind actions[BLHELI_S_BOOT_MAX_ACTIONS];
};

struct blheli_s_bootloader_entry_descriptor {
    bool rtx_high_for_boot_window;
    bool high_boot_start_variant;
};

struct blheli_s_bootloader_init_descriptor {
    bool enable_vdd_monitor;
};

struct blheli_s_bootloader_signature_descriptor {
    const uint8_t *bytes;
    size_t size;
    bool crc_ok;
    bool timed_out;
};

struct blheli_s_bootloader_command_descriptor {
    uint8_t command_high;
    uint8_t command_low;
    uint16_t argument;
    uint16_t current_address;
    uint16_t boot_start;
    bool crc_ok;
};

struct blheli_s_bootloader_result {
    enum blheli_s_bootloader_result_kind result;
    enum blheli_s_bootloader_command_kind command;
    uint8_t status_code;
    uint16_t boot_start;
    uint16_t address;
    uint16_t buffer_length;
    bool boot_info_response_intent;
    bool boot_area_rejected;
    bool flash_operation_intent;
    bool reads_flash_intent;
    bool reset_or_jump_intent;
    struct blheli_s_bootloader_trace trace;
};

const char *blheli_s_bootloader_signature(void);
uint16_t blheli_s_bootloader_start_address(bool high_boot_start_variant);
void blheli_s_bootloader_entry_decide(
    const struct blheli_s_bootloader_entry_descriptor *descriptor,
    struct blheli_s_bootloader_result *result);
void blheli_s_bootloader_init_trace(
    const struct blheli_s_bootloader_init_descriptor *descriptor,
    struct blheli_s_bootloader_result *result);
void blheli_s_bootloader_scan_signature(
    const struct blheli_s_bootloader_signature_descriptor *descriptor,
    struct blheli_s_bootloader_result *result);
void blheli_s_bootloader_decode_command(
    const struct blheli_s_bootloader_command_descriptor *descriptor,
    struct blheli_s_bootloader_result *result);

#endif
