#include <assert.h>

#include "core/pwm_control.h"

void test_pwm_control(void)
{
    struct blheli_s_pwm_request request;
    struct blheli_s_pwm_state state = {0};
    struct blheli_s_pwm_commit_result result;
    const struct blheli_s_pwm_parameters limited = {
        .pwm_limit = 100u,
        .pwm_limit_by_rpm = 80u,
        .requested_pwm = 120u,
        .clock_48mhz = false,
        .fet_on_delay = 2u
    };

    blheli_s_pwm_build_request(&limited, &request);
    assert(request.power_compare == 0x02bfu);
    assert(request.damping_compare == 0x02bdu);
    assert(request.damping_enabled);

    {
        const struct blheli_s_pwm_parameters high_clock = {
            .pwm_limit = 255u,
            .pwm_limit_by_rpm = 255u,
            .requested_pwm = 255u,
            .clock_48mhz = true,
            .fet_on_delay = 200u
        };
        blheli_s_pwm_build_request(&high_clock, &request);
        assert(request.power_compare == 0x0007u);
        assert(request.damping_compare == 0u);
    }

    {
        const struct blheli_s_pwm_parameters zero_pwm = {
            .pwm_limit = 255u,
            .pwm_limit_by_rpm = 255u,
            .requested_pwm = 0u,
            .clock_48mhz = false,
            .fet_on_delay = 0u
        };
        blheli_s_pwm_build_request(&zero_pwm, &request);
        assert(request.power_compare == 0x03ffu);
        assert(request.damping_compare == 0x03ffu);
        assert(!request.damping_enabled);
    }

    assert(blheli_s_pwm_stage_request(&state, &limited) ==
           BLHELI_S_PWM_INTERRUPT_COVF);
    assert(state.pending_valid);
    assert(state.pending.power_compare == 0x02bfu);
    assert(state.fet_on_delay_enabled);
    assert(!blheli_s_pwm_pca_window_allows_transfer(
        state.current_power_compare_high, 0x00u, state.clock_48mhz));
    result = blheli_s_pwm_commit_pending(&state, 0x00u);
    assert(!result.committed);
    assert(state.pending_valid);
    assert(blheli_s_pwm_pca_window_allows_transfer(
        state.current_power_compare_high, 0x02u, state.clock_48mhz));
    result = blheli_s_pwm_commit_pending(&state, 0x02u);
    assert(result.committed);
    assert(result.power_committed);
    assert(result.damping_committed);
    assert(result.disable_covf_interrupt);
    assert(!result.disable_ccf_interrupt);
    assert(!state.pending_valid);
    assert(state.current_power_compare_high == 0x02u);

    {
        const struct blheli_s_pwm_parameters no_delay = {
            .pwm_limit = 64u,
            .pwm_limit_by_rpm = 64u,
            .requested_pwm = 64u,
            .clock_48mhz = false,
            .fet_on_delay = 0u
        };
        assert(blheli_s_pwm_stage_request(&state, &no_delay) ==
               BLHELI_S_PWM_INTERRUPT_COVF);
        result = blheli_s_pwm_commit_pending(&state, 0x00u);
        assert(result.committed);
        assert(result.power_committed);
        assert(!result.damping_committed);
        assert(result.disable_covf_interrupt);
        assert(result.disable_ccf_interrupt);
    }

    {
        const struct blheli_s_pwm_parameters no_delay = {
            .pwm_limit = 64u,
            .pwm_limit_by_rpm = 64u,
            .requested_pwm = 64u,
            .clock_48mhz = false,
            .fet_on_delay = 0u
        };
        state.current_power_compare_high = 0x00u;
        assert(blheli_s_pwm_stage_request(&state, &no_delay) ==
               BLHELI_S_PWM_INTERRUPT_CCF);
        state.current_power_compare_high = 0x02u;
        assert(blheli_s_pwm_stage_request(&state, &no_delay) ==
               BLHELI_S_PWM_INTERRUPT_COVF);
    }

    assert(blheli_s_pwm_pca_window_allows_transfer(0x04u, 0x00u, true));
    assert(!blheli_s_pwm_pca_window_allows_transfer(0x04u, 0x06u, true));
    assert(blheli_s_pwm_commit_pending(0, 0x00u).committed == false);
}
