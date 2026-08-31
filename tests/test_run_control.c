#include <assert.h>

#include "core/run_control.h"

void test_run_control(void)
{
    struct blheli_s_run_state state;

    blheli_s_run_begin(&state, 160u);
    assert(state.step == BLHELI_S_COMMUTATION_STEP_1);
    assert(state.demag_metric == 120u);
    assert(blheli_s_run_update(&state, false, true, false) ==
           BLHELI_S_RUN_COMMUTATE);
    assert(state.step == BLHELI_S_COMMUTATION_STEP_2);
    assert(state.demag_metric == 120u);
    assert(blheli_s_run_update(&state, false, false, false) ==
           BLHELI_S_RUN_COMMUTATE);
    assert(state.demag_metric == 120u);
    assert(blheli_s_run_update(&state, true, false, false) ==
           BLHELI_S_RUN_SYNC_LOST);
    assert(blheli_s_run_update(&state, true, false, true) ==
           BLHELI_S_RUN_COMMUTATE);
}
