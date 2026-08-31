#include <assert.h>

#include "core/commutation.h"
#include "hal/phase_mapping.h"

void test_commutation(void)
{
    enum blheli_s_commutation_step step = BLHELI_S_COMMUTATION_STEP_1;
    const struct esc_hal_phase_mapping *mapping;

    for (unsigned int count = 0u; count < 6u; ++count) {
        assert(blheli_s_commutation_step_is_valid(step));
        step = blheli_s_commutation_next_step(step);
    }
    assert(step == BLHELI_S_COMMUTATION_STEP_1);
    assert(blheli_s_commutation_next_step(0) ==
           BLHELI_S_COMMUTATION_STEP_1);
    assert(blheli_s_commutation_rpm_output_enabled(
               BLHELI_S_COMMUTATION_STEP_2));
    assert(!blheli_s_commutation_rpm_output_enabled(
               BLHELI_S_COMMUTATION_STEP_1));

    mapping = esc_hal_get_phase_mapping(
        BLHELI_S_COMMUTATION_STEP_1, BLHELI_S_COMMUTATION_FORWARD);
    assert(mapping->pwm_phase == ESC_HAL_PHASE_B);
    assert(mapping->commutation_phase == ESC_HAL_PHASE_C);
    assert(mapping->bemf_phase == ESC_HAL_PHASE_A);

    mapping = esc_hal_get_phase_mapping(
        BLHELI_S_COMMUTATION_STEP_1, BLHELI_S_COMMUTATION_REVERSE);
    assert(mapping->pwm_phase == ESC_HAL_PHASE_A);
    assert(mapping->commutation_phase == ESC_HAL_PHASE_B);
    assert(mapping->bemf_phase == ESC_HAL_PHASE_C);
    assert(esc_hal_get_phase_mapping(0,
                                     BLHELI_S_COMMUTATION_FORWARD) == 0);
}
