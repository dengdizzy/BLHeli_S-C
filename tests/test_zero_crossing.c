#include <assert.h>

#include "core/zero_crossing.h"

void test_zero_crossing(void)
{
    struct blheli_s_zero_crossing_state state;
    struct blheli_s_zero_crossing_descriptor descriptor;

    blheli_s_zero_crossing_begin(&state, 100u, 10u, 30u);
    assert(state.timer_action == BLHELI_S_ZERO_CROSSING_TIMER_WAIT_SCAN);
    assert(blheli_s_zero_crossing_update(&state, 109u, true) ==
           BLHELI_S_ZERO_CROSSING_SCANNING);
    assert(!state.scan_started);
    assert(blheli_s_zero_crossing_update(&state, 110u, true) ==
           BLHELI_S_ZERO_CROSSING_READY);
    assert(state.scan_started);
    assert(state.comparator_read_count == 1u);
    assert(state.timer_action == BLHELI_S_ZERO_CROSSING_TIMER_SET_TIMEOUT);

    blheli_s_zero_crossing_begin(&state, 100u, 10u, 30u);
    assert(blheli_s_zero_crossing_update(&state, 130u, false) ==
           BLHELI_S_ZERO_CROSSING_TIMED_OUT);
    assert(blheli_s_zero_crossing_update(&state, 131u, true) ==
           BLHELI_S_ZERO_CROSSING_TIMED_OUT);

    descriptor = (struct blheli_s_zero_crossing_descriptor){
        .scan_delay = 10u,
        .base_timeout = 30u,
        .commutation_period_four_x = 0x0100u,
        .startup_timeout_extensions = 2u,
        .startup_phase = false,
        .initial_run_phase = true,
        .clock_48mhz = false
    };
    assert(blheli_s_zero_crossing_timeout_duration(&descriptor) == 0x0040u);
    blheli_s_zero_crossing_begin_from_descriptor(&state, 100u, &descriptor);
    assert(state.timeout_deadline == 164u);
    assert(state.startup_timeout_countdown == 2u);

    descriptor.startup_phase = true;
    descriptor.initial_run_phase = false;
    assert(blheli_s_zero_crossing_timeout_duration(&descriptor) == 0x4040u);
    blheli_s_zero_crossing_begin_from_descriptor(&state, 100u, &descriptor);
    assert(state.timeout_deadline == 0x40a4u);
    assert(blheli_s_zero_crossing_update(&state, 110u, false) ==
           BLHELI_S_ZERO_CROSSING_SCANNING);
    assert(state.comparator_read_count == 1u);
    assert(blheli_s_zero_crossing_update(&state, 0x40a4u, false) ==
           BLHELI_S_ZERO_CROSSING_SCANNING);
    assert(state.startup_timeout_countdown == 1u);
    assert(state.timer_action ==
           BLHELI_S_ZERO_CROSSING_TIMER_EXTEND_TIMEOUT);
    assert(blheli_s_zero_crossing_update(&state, 0x8148u, false) ==
           BLHELI_S_ZERO_CROSSING_TIMED_OUT);

    descriptor = (struct blheli_s_zero_crossing_descriptor){
        .scan_delay = 0u,
        .base_timeout = 0u,
        .commutation_period_four_x = 0x0100u,
        .startup_timeout_extensions = 2u,
        .startup_phase = false,
        .initial_run_phase = true,
        .clock_48mhz = true
    };
    assert(blheli_s_zero_crossing_timeout_duration(&descriptor) == 0x0080u);

    blheli_s_zero_crossing_begin(&state, 0xfffffff0u, 0x20u, 0x40u);
    assert(blheli_s_zero_crossing_update(&state, 0x0000000fu, true) ==
           BLHELI_S_ZERO_CROSSING_SCANNING);
    assert(blheli_s_zero_crossing_update(&state, 0x00000010u, true) ==
           BLHELI_S_ZERO_CROSSING_READY);
}
