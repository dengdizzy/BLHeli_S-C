#ifndef BLHELI_S_STORAGE_H
#define BLHELI_S_STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool storage_read(uint32_t offset, void *buffer, size_t size);
bool storage_replace(uint32_t offset, const void *buffer, size_t size);

#endif
