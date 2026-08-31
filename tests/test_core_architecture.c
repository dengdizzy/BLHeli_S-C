#include <assert.h>

#include "core/esc_control.h"

void test_core_architecture(void)
{
    struct blheli_s_core_state state;

    blheli_s_core_init(&state);
    assert(state.initialized);
    assert(state.phase == BLHELI_S_CONTROL_STOPPED);
    assert(state.reserved == 0u);

    state.phase = BLHELI_S_CONTROL_RUNNING;
    blheli_s_core_reset(&state);
    assert(!state.initialized);
    assert(state.phase == BLHELI_S_CONTROL_STOPPED);
}
