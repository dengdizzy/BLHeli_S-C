#include <assert.h>

#include "core/demag.h"

void test_demag(void)
{
    struct blheli_s_demag_descriptor descriptor = {
        .current_metric = 120u,
        .power_off_threshold = 160u,
        .commutation_period_four_x_high = 0x20u,
        .comparator_wrong_read = true,
        .startup_phase = false,
        .initial_run_phase = false,
        .high_rpm = false,
        .direction_change_brake = false,
        .clock_48mhz = false
    };
    struct blheli_s_demag_result result;
    struct blheli_s_demag_state state;

    blheli_s_demag_begin(&state, 160u);
    assert(state.metric == 120u);
    assert(!state.all_pwm_fets_off);
    assert(!state.set_pwms_off);
    assert(!state.requires_power_reapply);
    blheli_s_demag_update(&state, true);
    assert(state.detected);
    assert(state.metric == 137u);
    assert(!state.power_cut);
    for (unsigned int count = 0u; count < 8u; ++count) {
        blheli_s_demag_update(&state, true);
    }
    assert(state.power_cut);
    assert(state.all_pwm_fets_off);
    assert(state.set_pwms_off);
    assert(state.requires_power_reapply);
    blheli_s_demag_update(&state, false);
    assert(!state.detected);
    assert(state.metric >= 120u);

    assert(blheli_s_demag_next_metric(120u, false) == 120u);
    assert(blheli_s_demag_next_metric(120u, true) == 137u);

    blheli_s_demag_begin(&state, 130u);
    blheli_s_demag_update(&state, true);
    assert(state.power_cut);

    blheli_s_demag_begin(&state, 255u);
    for (unsigned int count = 0u; count < 32u; ++count) {
        blheli_s_demag_update(&state, true);
    }
    assert(!state.power_cut);

    blheli_s_demag_evaluate(&descriptor, &result);
    assert(!result.detected);
    assert(result.timeout_extension_requested);
    assert(result.comparator_wait_restarts);
    assert(result.timer_action == BLHELI_S_DEMAG_TIMER_EXTEND_LOW_RPM);
    assert(result.timer_reload_low == 0u);
    assert(result.timer_reload_high == 0x80u);
    assert(!result.power_cut);

    descriptor.clock_48mhz = true;
    blheli_s_demag_evaluate(&descriptor, &result);
    assert(result.timer_reload_high == 1u);

    descriptor.high_rpm = true;
    descriptor.clock_48mhz = false;
    blheli_s_demag_evaluate(&descriptor, &result);
    assert(result.timer_action == BLHELI_S_DEMAG_TIMER_EXTEND_HIGH_RPM);
    assert(result.timer_reload_high == 0xf8u);
    descriptor.clock_48mhz = true;
    blheli_s_demag_evaluate(&descriptor, &result);
    assert(result.timer_reload_high == 0xf0u);

    descriptor.high_rpm = false;
    descriptor.clock_48mhz = false;
    descriptor.startup_phase = true;
    blheli_s_demag_evaluate(&descriptor, &result);
    assert(!result.detected);
    assert(result.startup_or_initial_run_suppresses_demag);
    assert(!result.timeout_extension_requested);

    descriptor.startup_phase = false;
    descriptor.initial_run_phase = true;
    blheli_s_demag_evaluate(&descriptor, &result);
    assert(!result.detected);
    assert(result.startup_or_initial_run_suppresses_demag);

    descriptor.initial_run_phase = false;
    descriptor.comparator_wrong_read = false;
    descriptor.current_metric = 160u;
    blheli_s_demag_evaluate(&descriptor, &result);
    assert(result.detected);
    assert(result.power_cut);
    assert(result.all_pwm_fets_off);
    assert(result.set_pwms_off);
    assert(result.requires_power_reapply);
}
