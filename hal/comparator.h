#ifndef BLHELI_S_COMPARATOR_H
#define BLHELI_S_COMPARATOR_H

#include <stdbool.h>

bool esc_hal_comparator_read(void);
void esc_hal_comparator_select_phase(unsigned int phase);

#endif
