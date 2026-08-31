#include <assert.h>

#include "core/startup.h"

void test_startup(void)
{
    struct blheli_s_startup_state state;

    blheli_s_startup_begin(&state, 42u, true);
    assert(state.pwm_limit == 42u);
    assert(state.pwm_limit_by_rpm == 42u);
    assert(state.direction_reversed);
    assert(blheli_s_startup_after_commutation(&state, 1u) ==
           BLHELI_S_STARTUP_CONTINUE);
    for (unsigned int count = 1u; count < 24u; ++count) {
        assert(blheli_s_startup_after_commutation(&state, 1u) ==
               (count == 23u ? BLHELI_S_STARTUP_INITIAL_RUN
                             : BLHELI_S_STARTUP_CONTINUE));
    }
    assert(state.commutation_count == 24u);
    assert(state.initial_run_rotations_remaining == 12u);
    for (unsigned int count = 0u; count < 11u; ++count) {
        assert(blheli_s_startup_after_rotation(&state) ==
               BLHELI_S_STARTUP_INITIAL_RUN);
    }
    assert(blheli_s_startup_after_rotation(&state) ==
           BLHELI_S_STARTUP_CONTINUE);
    assert(state.initial_run_rotations_remaining == 0u);

    assert(blheli_s_startup_after_commutation(&state, 0u) ==
           BLHELI_S_STARTUP_STOP);
}
