#ifndef BLHELI_S_TIMING_DEADLINE_H
#define BLHELI_S_TIMING_DEADLINE_H

#include <stdbool.h>
#include <stdint.h>

bool esc_hal_timing_deadline_reached(uint32_t now, uint32_t deadline);
void esc_hal_timing_deadline_schedule(uint32_t deadline);

#endif
