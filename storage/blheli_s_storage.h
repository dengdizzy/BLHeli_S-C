#ifndef BLHELI_S_STORAGE_H
#define BLHELI_S_STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "config/blheli_s_config.h"

#define BLHELI_S_PARAMETER_RECORD_SIZE 58u
#define BLHELI_S_PARAMETER_NAME_OFFSET 42u

bool blheli_s_storage_header_valid(
    const uint8_t *record, size_t size);
bool blheli_s_storage_encode(
    const struct blheli_s_config *config, uint8_t *record, size_t size);
bool blheli_s_storage_decode(
    const uint8_t *record, size_t size, struct blheli_s_config *config);

#endif
