#include <assert.h>

#include "core/throttle_input.h"

void test_throttle_input(void)
{
    const struct blheli_s_throttle_input_config config = {
        .minimum = 1000u,
        .center = 1500u,
        .maximum = 2000u,
        .forward_gain = { 128u, 0u },
        .reverse_gain = { 128u, 0u },
        .bidirectional = true,
        .full_range = false,
        .bidirectional_reversed = false
    };
    const struct blheli_s_throttle_input_config unidirectional = {
        .minimum = 1000u,
        .center = 1500u,
        .maximum = 2000u,
        .forward_gain = { 128u, 0u },
        .reverse_gain = { 128u, 0u },
        .bidirectional = false,
        .full_range = false,
        .bidirectional_reversed = false
    };
    struct blheli_s_throttle_input_descriptor descriptor = {
        .protocol = BLHELI_S_THROTTLE_PROTOCOL_DSHOT,
        .config = unidirectional,
        .raw_capture = 1010u,
        .startup_pwm_limit_begin = 42u,
        .stall_count = 0u,
        .clock_48mhz = false,
        .startup_phase = true,
        .initial_run_phase = false,
        .motor_started = false
    };
    struct blheli_s_throttle_input_state state;
    struct blheli_s_throttle_input_result result;

    assert(blheli_s_throttle_scale_capture(
               BLHELI_S_THROTTLE_PROTOCOL_PPM, 24000u, false) == 1007u);
    assert(blheli_s_throttle_scale_capture(
               BLHELI_S_THROTTLE_PROTOCOL_PPM, 24000u, true) == 1007u);
    assert(blheli_s_throttle_scale_capture(
               BLHELI_S_THROTTLE_PROTOCOL_ONESHOT125, 3000u, false) ==
           1007u);
    assert(blheli_s_throttle_scale_capture(
               BLHELI_S_THROTTLE_PROTOCOL_ONESHOT42, 996u, false) == 1003u);
    assert(blheli_s_throttle_scale_capture(
               BLHELI_S_THROTTLE_PROTOCOL_MULTISHOT, 100u, false) == 977u);
    assert(blheli_s_throttle_scale_capture(
               BLHELI_S_THROTTLE_PROTOCOL_MULTISHOT, 100u, true) == 1745u);
    assert(blheli_s_throttle_scale_capture(
               BLHELI_S_THROTTLE_PROTOCOL_DSHOT, 1000u, true) == 1000u);

    result = blheli_s_process_throttle(1600u, &config);
    assert(result.value == 90u);
    assert(!result.direction_reversed);
    result = blheli_s_process_throttle(1400u, &config);
    assert(result.value == 390u);
    assert(result.direction_reversed);
    result = blheli_s_process_throttle(1505u, &config);
    assert(result.stop);
    result = blheli_s_process_throttle(2300u, &config);
    assert(result.outside_range);

    blheli_s_throttle_input_state_init(&state);
    result = blheli_s_process_throttle_from_descriptor(&descriptor, &state);
    assert(result.scaled_capture == 1010u);
    assert(result.value == 42u);
    assert(result.new_rcp == 42u);
    assert(result.new_rcp_updated);
    assert(result.startup_boost_applied);
    assert(result.timeout_reloaded);
    assert(state.new_rcp == 42u);
    assert(state.timeout_countdown == 10u);
    assert(state.stop_count == 0u);

    descriptor.clock_48mhz = true;
    descriptor.raw_capture = 1011u;
    result = blheli_s_process_throttle_from_descriptor(&descriptor, &state);
    assert(result.value == 84u);
    assert(result.startup_boost_applied);

    descriptor.startup_phase = false;
    descriptor.initial_run_phase = false;
    descriptor.raw_capture = 999u;
    result = blheli_s_process_throttle_from_descriptor(&descriptor, &state);
    assert(result.stop);
    assert(state.stop_count == 1u);
    assert(state.new_rcp == 0u);

    state.timeout_countdown = 2u;
    blheli_s_throttle_input_timer_tick(&state);
    assert(state.timeout_countdown == 1u);
    assert(state.stop_count == 2u);
    state.new_rcp = 5u;
    blheli_s_throttle_input_timer_tick(&state);
    assert(state.timeout_countdown == 0u);
    assert(state.stop_count == 0u);

    blheli_s_throttle_input_state_init(&state);
    descriptor.raw_capture = 2235u;
    for (unsigned int count = 1u; count < 50u; count++) {
        result = blheli_s_process_throttle_from_descriptor(&descriptor,
                                                           &state);
        assert(result.outside_range);
        assert(!result.new_rcp_cleared);
    }
    result = blheli_s_process_throttle_from_descriptor(&descriptor, &state);
    assert(result.outside_range);
    assert(result.new_rcp_cleared);
    assert(result.new_rcp_updated);
    assert(state.outside_range_count == 50u);
}
