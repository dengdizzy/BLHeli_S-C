#include <assert.h>

#include "core/zero_crossing.h"

void test_zero_crossing(void)
{
    struct blheli_s_zero_crossing_state state;

    blheli_s_zero_crossing_begin(&state, 100u, 10u, 30u);
    assert(blheli_s_zero_crossing_update(&state, 109u, true) ==
           BLHELI_S_ZERO_CROSSING_SCANNING);
    assert(blheli_s_zero_crossing_update(&state, 110u, true) ==
           BLHELI_S_ZERO_CROSSING_READY);

    blheli_s_zero_crossing_begin(&state, 100u, 10u, 30u);
    assert(blheli_s_zero_crossing_update(&state, 130u, false) ==
           BLHELI_S_ZERO_CROSSING_TIMED_OUT);
    assert(blheli_s_zero_crossing_update(&state, 131u, true) ==
           BLHELI_S_ZERO_CROSSING_TIMED_OUT);
}
