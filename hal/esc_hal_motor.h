#ifndef BLHELI_S_ESC_HAL_MOTOR_H
#define BLHELI_S_ESC_HAL_MOTOR_H

#include "hal/esc_hal.h"

void esc_hal_set_commutation_phase(enum esc_hal_phase phase);
void esc_hal_set_rpm_output(bool enabled);

#endif
