#ifndef BLHELI_S_ESC_HAL_STORAGE_H
#define BLHELI_S_ESC_HAL_STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool esc_hal_storage_read(uint32_t offset, void *buffer, size_t size);
bool esc_hal_storage_replace(uint32_t offset, const void *buffer, size_t size);

#endif
