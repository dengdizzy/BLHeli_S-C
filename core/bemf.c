#include "core/bemf.h"

void blheli_s_bemf_describe_wait(
    struct blheli_s_bemf_wait_descriptor *descriptor,
    enum blheli_s_bemf_wait wait, bool direction_change_brake,
    bool startup_phase, bool initial_run_phase, bool high_rpm,
    uint8_t commutation_period_four_x_high, bool clock_48mhz)
{
    uint8_t required = 1u;
    uint8_t maximum = 1u;

    if (!high_rpm) {
        maximum = 20u;
        required = (uint8_t)(commutation_period_four_x_high >> 1u);
        if (required == 0u) {
            required = 1u;
        }
        if (required > maximum) {
            required = maximum;
        }
        if (startup_phase) {
            required = 27u;
            maximum = 27u;
        }
    }
    if (clock_48mhz) {
        required = (uint8_t)(required << 1u);
        maximum = (uint8_t)(maximum << 1u);
    }

    *descriptor = (struct blheli_s_bemf_wait_descriptor){
        .wait = wait,
        .required_stable_samples = required,
        .maximum_required_samples = maximum,
        .expected_high =
            wait == BLHELI_S_BEMF_WAIT_HIGH ? true : false,
        .initial_demag_detected = !(startup_phase || initial_run_phase)
    };
    if (direction_change_brake) {
        descriptor->expected_high = !descriptor->expected_high;
    }
}

bool blheli_s_bemf_expected_high_for_run_transition(
    enum blheli_s_run_comparator_transition transition,
    bool direction_change_brake)
{
    bool expected_high =
        transition == BLHELI_S_RUN_COMPARATOR_LOW_TO_HIGH;

    return direction_change_brake ? !expected_high : expected_high;
}

void blheli_s_bemf_begin(struct blheli_s_bemf_sample_state *state,
                         bool expected_high, uint8_t required)
{
    *state = (struct blheli_s_bemf_sample_state){
        .reads = 0u,
        .stable_reads = 0u,
        .required = required == 0u ? 1u : required,
        .maximum_required = required == 0u ? 1u : required,
        .expected_high = expected_high,
        .demag_detected = true,
        .timed_out = false
    };
}

void blheli_s_bemf_begin_from_descriptor(
    struct blheli_s_bemf_sample_state *state,
    const struct blheli_s_bemf_wait_descriptor *descriptor)
{
    *state = (struct blheli_s_bemf_sample_state){
        .reads = 0u,
        .stable_reads = 0u,
        .required = descriptor->required_stable_samples == 0u
            ? 1u : descriptor->required_stable_samples,
        .maximum_required = descriptor->maximum_required_samples == 0u
            ? 1u : descriptor->maximum_required_samples,
        .expected_high = descriptor->expected_high,
        .demag_detected = descriptor->initial_demag_detected,
        .timed_out = false
    };
}

enum blheli_s_bemf_result blheli_s_bemf_sample(
    struct blheli_s_bemf_sample_state *state, bool comparator_high)
{
    return blheli_s_bemf_sample_with_timeout(state, comparator_high, false);
}

enum blheli_s_bemf_result blheli_s_bemf_sample_with_timeout(
    struct blheli_s_bemf_sample_state *state, bool comparator_high,
    bool timed_out)
{
    if (state->timed_out) {
        return BLHELI_S_BEMF_TIMED_OUT;
    }
    if (timed_out && state->reads != 0u) {
        state->timed_out = true;
        return BLHELI_S_BEMF_TIMED_OUT;
    }

    state->reads++;
    if (comparator_high != state->expected_high) {
        state->demag_detected = false;
        state->stable_reads = 0u;
        return BLHELI_S_BEMF_REJECTED;
    }
    state->stable_reads++;
    if (state->stable_reads < state->required) {
        return BLHELI_S_BEMF_WAITING;
    }
    return BLHELI_S_BEMF_ACCEPTED;
}
