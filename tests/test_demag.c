#include <assert.h>

#include "core/demag.h"

void test_demag(void)
{
    struct blheli_s_demag_state state;

    blheli_s_demag_begin(&state, 160u);
    assert(state.metric == 120u);
    blheli_s_demag_update(&state, true);
    assert(state.detected);
    assert(state.metric == 137u);
    assert(!state.power_cut);
    for (unsigned int count = 0u; count < 8u; ++count) {
        blheli_s_demag_update(&state, true);
    }
    assert(state.power_cut);
    blheli_s_demag_update(&state, false);
    assert(!state.detected);
    assert(state.metric >= 120u);
}
