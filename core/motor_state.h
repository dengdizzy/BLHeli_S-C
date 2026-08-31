#ifndef BLHELI_S_MOTOR_STATE_H
#define BLHELI_S_MOTOR_STATE_H

#include <stdbool.h>
#include <stdint.h>

struct blheli_s_motor_state {
    uint8_t startup_count;
    uint8_t startup_zc_timeout_countdown;
    uint8_t initial_run_rotation_countdown;
    uint8_t stall_count;
    uint8_t new_rcp;
    uint8_t rcp_stop_count;
    uint8_t power_pwm_limit;
    uint8_t power_pwm_limit_by_rpm;
    uint8_t power_pwm_limit_begin;
    uint8_t power_pwm_current_high;
    bool direction_reversed;
    bool direction_change_brake;
    bool motor_started;
    bool high_rpm;
};

#endif
