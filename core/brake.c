#include "core/brake.h"

void blheli_s_brake_begin(struct blheli_s_brake_state *state,
                          bool brake_on_stop)
{
    *state = (struct blheli_s_brake_state){
        .direction_change = false,
        .brake_on_stop = brake_on_stop,
        .brake_active = false,
        .stop_count = 0u,
        .speed_period = 0u
    };
}

enum blheli_s_brake_result blheli_s_brake_update(
    struct blheli_s_brake_state *state, uint8_t stop_count,
    bool requested_reverse, bool actual_reverse, uint16_t speed_period)
{
    state->stop_count = stop_count;
    state->speed_period = speed_period;

    if (requested_reverse != actual_reverse) {
        state->direction_change = true;
        state->brake_active = true;
        return speed_period > 0x20u ? BLHELI_S_BRAKE_REVERSE_START
                                    : BLHELI_S_BRAKE_RUN;
    }
    if (stop_count >= (state->brake_on_stop ? 3u : 250u)) {
        state->brake_active = state->brake_on_stop;
        return BLHELI_S_BRAKE_STOP;
    }
    return BLHELI_S_BRAKE_RUN;
}
