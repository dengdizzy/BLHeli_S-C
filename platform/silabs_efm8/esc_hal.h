#ifndef BLHELI_S_SILABS_EFM8_ESC_HAL_H
#define BLHELI_S_SILABS_EFM8_ESC_HAL_H

#include <stdbool.h>
#include <stdint.h>

struct silabs_efm8_registers {
    uint8_t timer0_mode;
    uint8_t timer1_mode;
    uint8_t timer2_control;
    uint8_t timer3_control;
    uint8_t pca_control;
    uint8_t pca_pwm_mode;
    uint8_t pca_polarity;
    uint8_t comparator_control;
    uint8_t adc_config;
    uint8_t adc_control;
    bool interrupts_enabled;
};

bool silabs_efm8_initialize(struct silabs_efm8_registers *registers);

#endif
