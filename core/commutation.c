#include "core/commutation.h"

enum blheli_s_commutation_step blheli_s_commutation_next_step(
    enum blheli_s_commutation_step step)
{
    if (step == BLHELI_S_COMMUTATION_STEP_6) {
        return BLHELI_S_COMMUTATION_STEP_1;
    }
    if (blheli_s_commutation_step_is_valid(step)) {
        return (enum blheli_s_commutation_step)((uint8_t)step + 1u);
    }
    return BLHELI_S_COMMUTATION_STEP_1;
}

bool blheli_s_commutation_step_is_valid(
    enum blheli_s_commutation_step step)
{
    return step >= BLHELI_S_COMMUTATION_STEP_1 &&
           step <= BLHELI_S_COMMUTATION_STEP_6;
}

bool blheli_s_commutation_rpm_output_enabled(
    enum blheli_s_commutation_step step)
{
    return step == BLHELI_S_COMMUTATION_STEP_2 ||
           step == BLHELI_S_COMMUTATION_STEP_4 ||
           step == BLHELI_S_COMMUTATION_STEP_6;
}
