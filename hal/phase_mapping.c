#include "hal/phase_mapping.h"

static const struct esc_hal_phase_mapping forward_mapping[] = {
    { ESC_HAL_PHASE_B, ESC_HAL_PHASE_C, ESC_HAL_PHASE_A },
    { ESC_HAL_PHASE_A, ESC_HAL_PHASE_C, ESC_HAL_PHASE_B },
    { ESC_HAL_PHASE_A, ESC_HAL_PHASE_B, ESC_HAL_PHASE_C },
    { ESC_HAL_PHASE_C, ESC_HAL_PHASE_B, ESC_HAL_PHASE_A },
    { ESC_HAL_PHASE_C, ESC_HAL_PHASE_A, ESC_HAL_PHASE_B },
    { ESC_HAL_PHASE_B, ESC_HAL_PHASE_A, ESC_HAL_PHASE_C }
};

static const struct esc_hal_phase_mapping reverse_mapping[] = {
    { ESC_HAL_PHASE_A, ESC_HAL_PHASE_B, ESC_HAL_PHASE_C },
    { ESC_HAL_PHASE_A, ESC_HAL_PHASE_C, ESC_HAL_PHASE_B },
    { ESC_HAL_PHASE_B, ESC_HAL_PHASE_C, ESC_HAL_PHASE_A },
    { ESC_HAL_PHASE_B, ESC_HAL_PHASE_A, ESC_HAL_PHASE_C },
    { ESC_HAL_PHASE_C, ESC_HAL_PHASE_A, ESC_HAL_PHASE_B },
    { ESC_HAL_PHASE_A, ESC_HAL_PHASE_B, ESC_HAL_PHASE_C }
};

const struct esc_hal_phase_mapping *esc_hal_get_phase_mapping(
    enum blheli_s_commutation_step step,
    enum blheli_s_commutation_direction direction)
{
    if (!blheli_s_commutation_step_is_valid(step)) {
        return 0;
    }
    if (direction == BLHELI_S_COMMUTATION_REVERSE) {
        return &reverse_mapping[(uint8_t)step - 1u];
    }
    return &forward_mapping[(uint8_t)step - 1u];
}
