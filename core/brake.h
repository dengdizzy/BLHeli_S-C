#ifndef BLHELI_S_BRAKE_H
#define BLHELI_S_BRAKE_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_brake_result {
    BLHELI_S_BRAKE_RUN,
    BLHELI_S_BRAKE_STOP,
    BLHELI_S_BRAKE_REVERSE_START
};

struct blheli_s_brake_state {
    bool direction_change;
    bool brake_on_stop;
    bool brake_active;
    uint8_t stop_count;
    uint16_t speed_period;
};

void blheli_s_brake_begin(struct blheli_s_brake_state *state,
                          bool brake_on_stop);
enum blheli_s_brake_result blheli_s_brake_update(
    struct blheli_s_brake_state *state, uint8_t stop_count,
    bool requested_reverse, bool actual_reverse, uint16_t speed_period);

#endif
