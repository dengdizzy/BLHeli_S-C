#include <assert.h>

#include "core/bemf.h"

void test_bemf(void)
{
    struct blheli_s_bemf_sample_state state;

    blheli_s_bemf_begin(&state, true, 2u);
    assert(blheli_s_bemf_sample(&state, true) == BLHELI_S_BEMF_WAITING);
    assert(blheli_s_bemf_sample(&state, true) == BLHELI_S_BEMF_ACCEPTED);
    assert(state.demag_detected);

    blheli_s_bemf_begin(&state, false, 2u);
    assert(blheli_s_bemf_sample(&state, true) == BLHELI_S_BEMF_REJECTED);
    assert(!state.demag_detected);

    blheli_s_bemf_begin(&state, true, 0u);
    assert(blheli_s_bemf_sample(&state, true) == BLHELI_S_BEMF_ACCEPTED);
    state.timed_out = true;
    assert(blheli_s_bemf_sample(&state, true) == BLHELI_S_BEMF_TIMED_OUT);
}
