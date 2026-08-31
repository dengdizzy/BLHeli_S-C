#ifndef BLHELI_S_STARTUP_H
#define BLHELI_S_STARTUP_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_startup_result {
    BLHELI_S_STARTUP_CONTINUE,
    BLHELI_S_STARTUP_INITIAL_RUN,
    BLHELI_S_STARTUP_STOP
};

struct blheli_s_startup_state {
    uint8_t commutation_count;
    uint8_t initial_run_rotations_remaining;
    uint8_t pwm_limit_begin;
    uint8_t pwm_limit;
    uint8_t pwm_limit_by_rpm;
    bool direction_reversed;
};

void blheli_s_startup_begin(struct blheli_s_startup_state *state,
                            uint8_t startup_pwm_limit,
                            bool direction_reversed);
enum blheli_s_startup_result blheli_s_startup_after_commutation(
    struct blheli_s_startup_state *state, uint8_t throttle);
enum blheli_s_startup_result blheli_s_startup_after_rotation(
    struct blheli_s_startup_state *state);

#endif
