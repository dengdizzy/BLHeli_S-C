#ifndef BLHELI_S_PWM_CONTROL_H
#define BLHELI_S_PWM_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

struct blheli_s_pwm_request {
    uint16_t power_compare;
    uint16_t damping_compare;
    bool damping_enabled;
};

struct blheli_s_pwm_parameters {
    uint8_t pwm_limit;
    uint8_t pwm_limit_by_rpm;
    uint8_t requested_pwm;
    bool clock_48mhz;
    uint8_t fet_on_delay;
};

enum blheli_s_pwm_interrupt {
    BLHELI_S_PWM_INTERRUPT_NONE,
    BLHELI_S_PWM_INTERRUPT_COVF,
    BLHELI_S_PWM_INTERRUPT_CCF
};

struct blheli_s_pwm_state {
    struct blheli_s_pwm_request pending;
    struct blheli_s_pwm_request current;
    uint8_t current_power_compare_high;
    bool pending_valid;
    bool clock_48mhz;
    bool fet_on_delay_enabled;
};

struct blheli_s_pwm_commit_result {
    bool committed;
    bool power_committed;
    bool damping_committed;
    bool disable_covf_interrupt;
    bool disable_ccf_interrupt;
};

void blheli_s_pwm_build_request(
    const struct blheli_s_pwm_parameters *parameters,
    struct blheli_s_pwm_request *request);
enum blheli_s_pwm_interrupt blheli_s_pwm_stage_request(
    struct blheli_s_pwm_state *state,
    const struct blheli_s_pwm_parameters *parameters);
bool blheli_s_pwm_pca_window_allows_transfer(
    uint8_t current_power_compare_high, uint8_t pca_counter_high,
    bool clock_48mhz);
struct blheli_s_pwm_commit_result blheli_s_pwm_commit_pending(
    struct blheli_s_pwm_state *state, uint8_t pca_counter_high);

#endif
