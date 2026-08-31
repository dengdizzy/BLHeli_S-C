#ifndef BLHELI_S_ESC_HAL_ADC_H
#define BLHELI_S_ESC_HAL_ADC_H

#include <stdbool.h>
#include <stdint.h>

bool esc_hal_adc_start(void);
bool esc_hal_adc_read(uint16_t *value);
void esc_hal_adc_stop(void);

#endif
