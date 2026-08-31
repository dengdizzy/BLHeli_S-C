#ifndef BLHELI_S_ESC_CONTROL_H
#define BLHELI_S_ESC_CONTROL_H

#include "core/esc_state.h"

enum blheli_s_core_update_result {
    BLHELI_S_CORE_WAITING,
    BLHELI_S_CORE_COMMUTATE,
    BLHELI_S_CORE_INITIAL_RUN,
    BLHELI_S_CORE_RUNNING,
    BLHELI_S_CORE_STOPPED,
    BLHELI_S_CORE_SYNC_LOST
};

struct blheli_s_core_run_event {
    uint8_t throttle;
    bool zero_cross_timeout;
    bool demag_detected;
    bool direction_change_brake;
};

void blheli_s_core_init(struct blheli_s_core_state *state);
void blheli_s_core_reset(struct blheli_s_core_state *state);
void blheli_s_core_begin_startup(struct blheli_s_core_state *state,
                                 uint8_t startup_pwm_limit,
                                 bool direction_reversed,
                                 uint8_t demag_power_off_threshold);
enum blheli_s_core_update_result blheli_s_core_after_run_event(
    struct blheli_s_core_state *state,
    const struct blheli_s_core_run_event *event);

#endif
