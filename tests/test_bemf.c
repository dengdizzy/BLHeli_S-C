#include <assert.h>

#include "core/bemf.h"
#include "core/run_control.h"

void test_bemf(void)
{
    struct blheli_s_bemf_sample_state state;
    struct blheli_s_bemf_wait_descriptor descriptor;
    const struct blheli_s_run_step_descriptor *run_step;

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

    blheli_s_bemf_describe_wait(&descriptor, BLHELI_S_BEMF_WAIT_HIGH,
                                false, false, false, true, 0u, false);
    assert(descriptor.expected_high);
    assert(descriptor.required_stable_samples == 1u);
    assert(descriptor.maximum_required_samples == 1u);
    assert(descriptor.initial_demag_detected);

    blheli_s_bemf_describe_wait(&descriptor, BLHELI_S_BEMF_WAIT_LOW,
                                true, false, false, true, 0u, false);
    assert(descriptor.expected_high);

    blheli_s_bemf_describe_wait(&descriptor, BLHELI_S_BEMF_WAIT_LOW,
                                false, false, false, false, 10u, false);
    assert(!descriptor.expected_high);
    assert(descriptor.required_stable_samples == 5u);
    assert(descriptor.maximum_required_samples == 20u);
    blheli_s_bemf_begin_from_descriptor(&state, &descriptor);
    assert(!state.expected_high);
    assert(state.required == 5u);
    assert(blheli_s_bemf_sample(&state, false) == BLHELI_S_BEMF_WAITING);
    assert(blheli_s_bemf_sample(&state, true) == BLHELI_S_BEMF_REJECTED);
    assert(!state.demag_detected);
    assert(state.stable_reads == 0u);

    blheli_s_bemf_describe_wait(&descriptor, BLHELI_S_BEMF_WAIT_HIGH,
                                false, true, false, false, 0xffu, true);
    assert(descriptor.expected_high);
    assert(descriptor.required_stable_samples == 54u);
    assert(descriptor.maximum_required_samples == 54u);
    assert(!descriptor.initial_demag_detected);

    blheli_s_bemf_begin_from_descriptor(&state, &descriptor);
    assert(blheli_s_bemf_sample_with_timeout(&state, true, true) ==
           BLHELI_S_BEMF_WAITING);
    assert(blheli_s_bemf_sample_with_timeout(&state, true, true) ==
           BLHELI_S_BEMF_TIMED_OUT);

    run_step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_1);
    assert(run_step != 0);
    assert(blheli_s_bemf_expected_high_for_run_transition(
        run_step->comparator_transition, false));
    assert(!blheli_s_bemf_expected_high_for_run_transition(
        run_step->comparator_transition, true));

    run_step = blheli_s_run_step_descriptor(BLHELI_S_COMMUTATION_STEP_2);
    assert(run_step != 0);
    assert(!blheli_s_bemf_expected_high_for_run_transition(
        run_step->comparator_transition, false));
}
