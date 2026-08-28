#ifndef BLHELI_S_PHASE_MAPPING_H
#define BLHELI_S_PHASE_MAPPING_H

#include "core/commutation.h"
#include "hal/esc_hal.h"

struct esc_hal_phase_mapping {
    enum esc_hal_phase pwm_phase;
    enum esc_hal_phase commutation_phase;
    enum esc_hal_phase bemf_phase;
};

const struct esc_hal_phase_mapping *esc_hal_get_phase_mapping(
    enum blheli_s_commutation_step step,
    enum blheli_s_commutation_direction direction);

#endif
