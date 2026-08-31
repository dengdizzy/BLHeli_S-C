#ifndef BLHELI_S_ESC_STATE_H
#define BLHELI_S_ESC_STATE_H

#include "core/esc_types.h"
#include "core/input_state.h"
#include "core/motor_state.h"
#include "core/protection_state.h"
#include "core/run_control.h"
#include "core/startup.h"
#include "core/timing_state.h"

struct blheli_s_core_state {
    enum blheli_s_control_phase phase;
    bool initialized;
    uint8_t reserved;
    struct blheli_s_motor_state motor;
    struct blheli_s_timing_state timing;
    struct blheli_s_input_state input;
    struct blheli_s_protection_state protection;
    struct blheli_s_startup_state startup;
    struct blheli_s_run_state run;
};

#endif
