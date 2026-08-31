#ifndef BLHELI_S_COMMUTATION_H
#define BLHELI_S_COMMUTATION_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_commutation_step {
    BLHELI_S_COMMUTATION_STEP_1 = 1u,
    BLHELI_S_COMMUTATION_STEP_2,
    BLHELI_S_COMMUTATION_STEP_3,
    BLHELI_S_COMMUTATION_STEP_4,
    BLHELI_S_COMMUTATION_STEP_5,
    BLHELI_S_COMMUTATION_STEP_6
};

enum blheli_s_commutation_direction {
    BLHELI_S_COMMUTATION_FORWARD,
    BLHELI_S_COMMUTATION_REVERSE
};

enum blheli_s_commutation_step blheli_s_commutation_next_step(
    enum blheli_s_commutation_step step);
bool blheli_s_commutation_step_is_valid(
    enum blheli_s_commutation_step step);
bool blheli_s_commutation_rpm_output_enabled(
    enum blheli_s_commutation_step step);

#endif
