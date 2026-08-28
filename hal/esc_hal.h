#ifndef BLHELI_S_ESC_HAL_H
#define BLHELI_S_ESC_HAL_H

#include <stdbool.h>
#include <stdint.h>

enum esc_hal_phase { ESC_HAL_PHASE_A, ESC_HAL_PHASE_B, ESC_HAL_PHASE_C };

struct esc_hal_pwm_request {
    uint16_t power_compare;
    uint16_t damping_compare;
    bool damping_enabled;
};

void esc_hal_pwm_commit(const struct esc_hal_pwm_request *request);
void esc_hal_gate_disable_all(void);
void esc_hal_select_bemf_phase(enum esc_hal_phase phase);
bool esc_hal_read_comparator(void);
uint32_t esc_hal_time_now(void);
void esc_hal_schedule_deadline(uint32_t tick);

#endif
