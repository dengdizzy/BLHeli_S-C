#ifndef BLHELI_S_ESC_HAL_INPUT_H
#define BLHELI_S_ESC_HAL_INPUT_H

#include <stdbool.h>
#include <stdint.h>

struct esc_hal_input_capture {
    uint32_t timestamp;
    uint16_t width;
};

bool esc_hal_input_capture_read(struct esc_hal_input_capture *capture);
void esc_hal_input_capture_enable(bool enabled);

#endif
