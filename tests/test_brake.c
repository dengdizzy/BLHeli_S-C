#include <assert.h>

#include "core/brake.h"

void test_brake(void)
{
    struct blheli_s_brake_state state;

    blheli_s_brake_begin(&state, true);
    assert(blheli_s_brake_update(&state, 2u, false, false, 0xf0u) ==
           BLHELI_S_BRAKE_RUN);
    assert(blheli_s_brake_update(&state, 3u, false, false, 0xf0u) ==
           BLHELI_S_BRAKE_STOP);
    assert(state.brake_active);

    blheli_s_brake_begin(&state, false);
    assert(blheli_s_brake_update(&state, 0u, true, false, 0x40u) ==
           BLHELI_S_BRAKE_REVERSE_START);
    assert(state.direction_change);
    assert(blheli_s_brake_update(&state, 0u, true, false, 0x20u) ==
           BLHELI_S_BRAKE_RUN);
}
