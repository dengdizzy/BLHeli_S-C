#ifndef BLHELI_S_ESC_HAL_PLATFORM_H
#define BLHELI_S_ESC_HAL_PLATFORM_H

#include <stdbool.h>

bool esc_hal_platform_initialize(void);
void esc_hal_platform_set_interrupts_enabled(bool enabled);

#endif
