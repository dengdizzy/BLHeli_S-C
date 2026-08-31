#include "core/demag.h"

void blheli_s_demag_begin(struct blheli_s_demag_state *state,
                          uint8_t power_off_threshold)
{
    *state = (struct blheli_s_demag_state){
        .metric = 120u,
        .power_off_threshold = power_off_threshold,
        .detected = false,
        .power_cut = false,
        .all_pwm_fets_off = false,
        .set_pwms_off = false,
        .requires_power_reapply = false
    };
}

uint8_t blheli_s_demag_next_metric(uint8_t metric, bool detected)
{
    uint16_t next = (uint16_t)metric * 7u;

    if (detected) {
        next = (uint16_t)(next + 256u);
    }
    metric = (uint8_t)(next >> 3u);
    return metric < 120u ? 120u : metric;
}

static uint8_t low_rpm_timeout_reload_high(uint8_t commutation_period_high,
                                           bool clock_48mhz)
{
    uint16_t timeout_high = commutation_period_high;

    timeout_high <<= clock_48mhz ? 3u : 2u;
    if (timeout_high > UINT8_MAX) {
        timeout_high = UINT8_MAX;
    }
    return (uint8_t)(0u - (uint8_t)timeout_high);
}

void blheli_s_demag_update(struct blheli_s_demag_state *state,
                           bool detected)
{
    state->detected = detected;
    state->metric = blheli_s_demag_next_metric(state->metric, detected);
    state->power_cut = state->metric >= state->power_off_threshold;
    state->all_pwm_fets_off = state->power_cut;
    state->set_pwms_off = state->power_cut;
    state->requires_power_reapply = state->power_cut;
}

void blheli_s_demag_evaluate(
    const struct blheli_s_demag_descriptor *descriptor,
    struct blheli_s_demag_result *result)
{
    bool start_phase = descriptor->startup_phase ||
        descriptor->initial_run_phase;
    bool detected = !start_phase;

    *result = (struct blheli_s_demag_result){
        .metric = descriptor->current_metric,
        .timer_reload_low = 0u,
        .timer_reload_high = 0u,
        .timer_action = BLHELI_S_DEMAG_TIMER_NONE,
        .detected = detected,
        .startup_or_initial_run_suppresses_demag = start_phase,
        .sync_loss_suppressed = descriptor->direction_change_brake || detected
    };

    if (descriptor->comparator_wrong_read && !start_phase && detected) {
        result->detected = false;
        result->timeout_extension_requested = true;
        result->comparator_wait_restarts = true;
        result->timer_action = descriptor->high_rpm
            ? BLHELI_S_DEMAG_TIMER_EXTEND_HIGH_RPM
            : BLHELI_S_DEMAG_TIMER_EXTEND_LOW_RPM;
        result->timer_reload_high = descriptor->high_rpm
            ? (descriptor->clock_48mhz ? 0xf0u : 0xf8u)
            : low_rpm_timeout_reload_high(
                  descriptor->commutation_period_four_x_high,
                  descriptor->clock_48mhz);
    }

    result->metric = blheli_s_demag_next_metric(descriptor->current_metric,
                                                result->detected);
    result->power_cut = result->metric >= descriptor->power_off_threshold;
    result->all_pwm_fets_off = result->power_cut;
    result->set_pwms_off = result->power_cut;
    result->requires_power_reapply = result->power_cut;
}
