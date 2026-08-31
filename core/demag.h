#ifndef BLHELI_S_DEMAG_H
#define BLHELI_S_DEMAG_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_demag_timer_action {
    BLHELI_S_DEMAG_TIMER_NONE,
    BLHELI_S_DEMAG_TIMER_EXTEND_HIGH_RPM,
    BLHELI_S_DEMAG_TIMER_EXTEND_LOW_RPM
};

struct blheli_s_demag_state {
    uint8_t metric;
    uint8_t power_off_threshold;
    bool detected;
    bool power_cut;
    bool all_pwm_fets_off;
    bool set_pwms_off;
    bool requires_power_reapply;
};

struct blheli_s_demag_descriptor {
    uint8_t current_metric;
    uint8_t power_off_threshold;
    uint8_t commutation_period_four_x_high;
    bool comparator_wrong_read;
    bool startup_phase;
    bool initial_run_phase;
    bool high_rpm;
    bool direction_change_brake;
    bool clock_48mhz;
};

struct blheli_s_demag_result {
    uint8_t metric;
    uint8_t timer_reload_low;
    uint8_t timer_reload_high;
    enum blheli_s_demag_timer_action timer_action;
    bool detected;
    bool timeout_extension_requested;
    bool comparator_wait_restarts;
    bool startup_or_initial_run_suppresses_demag;
    bool sync_loss_suppressed;
    bool power_cut;
    bool all_pwm_fets_off;
    bool set_pwms_off;
    bool requires_power_reapply;
};

void blheli_s_demag_begin(struct blheli_s_demag_state *state,
                          uint8_t power_off_threshold);
void blheli_s_demag_update(struct blheli_s_demag_state *state,
                           bool detected);
uint8_t blheli_s_demag_next_metric(uint8_t metric, bool detected);
void blheli_s_demag_evaluate(
    const struct blheli_s_demag_descriptor *descriptor,
    struct blheli_s_demag_result *result);

#endif
