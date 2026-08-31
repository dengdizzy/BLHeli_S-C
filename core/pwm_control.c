#include "core/pwm_control.h"

static uint16_t blheli_s_pwm_compare(uint8_t limit, bool clock_48mhz)
{
    uint16_t scaled = (uint16_t)limit * (clock_48mhz ? 8u : 4u);
    uint16_t high_mask = clock_48mhz ? 7u : 3u;

    return (uint16_t)((uint16_t)~scaled &
                      (uint16_t)(0x00ffu | (uint16_t)(high_mask << 8u)));
}

static uint8_t blheli_s_pwm_compare_high(uint16_t compare)
{
    return (uint8_t)(compare >> 8u);
}

static bool blheli_s_pwm_current_is_high_pwm(uint8_t current_power_compare_high,
                                             bool clock_48mhz)
{
    uint8_t threshold_bit = clock_48mhz ? 0x04u : 0x02u;

    return (current_power_compare_high & threshold_bit) == 0u;
}

void blheli_s_pwm_build_request(
    const struct blheli_s_pwm_parameters *parameters,
    struct blheli_s_pwm_request *request)
{
    uint8_t limit = parameters->pwm_limit;
    uint8_t pwm;
    uint16_t power_compare;
    uint16_t damping_compare;
    uint16_t delay;

    if (parameters->pwm_limit_by_rpm < limit) {
        limit = parameters->pwm_limit_by_rpm;
    }
    pwm = parameters->requested_pwm;
    if (limit < pwm) {
        pwm = limit;
    }

    power_compare = blheli_s_pwm_compare(pwm, parameters->clock_48mhz);
    damping_compare = power_compare;
    delay = (uint16_t)parameters->fet_on_delay *
            (parameters->clock_48mhz ? 2u : 1u);
    if (delay > damping_compare) {
        damping_compare = 0u;
    } else {
        damping_compare = (uint16_t)(damping_compare - delay);
    }

    *request = (struct blheli_s_pwm_request){
        .power_compare = power_compare,
        .damping_compare = damping_compare,
        .damping_enabled = parameters->fet_on_delay != 0u
    };
}

enum blheli_s_pwm_interrupt blheli_s_pwm_stage_request(
    struct blheli_s_pwm_state *state,
    const struct blheli_s_pwm_parameters *parameters)
{
    if (state == 0 || parameters == 0) {
        return BLHELI_S_PWM_INTERRUPT_NONE;
    }

    blheli_s_pwm_build_request(parameters, &state->pending);
    state->pending_valid = true;
    state->clock_48mhz = parameters->clock_48mhz;
    state->fet_on_delay_enabled = parameters->fet_on_delay != 0u;

    if (state->fet_on_delay_enabled) {
        return BLHELI_S_PWM_INTERRUPT_COVF;
    }
    return blheli_s_pwm_current_is_high_pwm(
        state->current_power_compare_high, state->clock_48mhz)
        ? BLHELI_S_PWM_INTERRUPT_CCF
        : BLHELI_S_PWM_INTERRUPT_COVF;
}

bool blheli_s_pwm_pca_window_allows_transfer(
    uint8_t current_power_compare_high, uint8_t pca_counter_high,
    bool clock_48mhz)
{
    uint8_t threshold_bit = clock_48mhz ? 0x04u : 0x02u;
    uint8_t lower_bit = clock_48mhz ? 0x02u : 0x01u;
    bool high_pwm = blheli_s_pwm_current_is_high_pwm(
        current_power_compare_high, clock_48mhz);

    if (high_pwm) {
        return (pca_counter_high & threshold_bit) != 0u &&
               (pca_counter_high & lower_bit) == 0u;
    }
    return (pca_counter_high & threshold_bit) == 0u &&
           (pca_counter_high & lower_bit) == 0u;
}

struct blheli_s_pwm_commit_result blheli_s_pwm_commit_pending(
    struct blheli_s_pwm_state *state, uint8_t pca_counter_high)
{
    struct blheli_s_pwm_commit_result result = {0};

    if (state == 0 || !state->pending_valid) {
        return result;
    }
    if (state->fet_on_delay_enabled &&
        !blheli_s_pwm_pca_window_allows_transfer(
            state->current_power_compare_high, pca_counter_high,
            state->clock_48mhz)) {
        return result;
    }

    state->current = state->pending;
    state->current_power_compare_high =
        blheli_s_pwm_compare_high(state->pending.power_compare);
    state->pending_valid = false;
    result.committed = true;
    result.power_committed = true;
    result.damping_committed = state->pending.damping_enabled;
    result.disable_covf_interrupt = true;
    result.disable_ccf_interrupt = !state->pending.damping_enabled;
    return result;
}
