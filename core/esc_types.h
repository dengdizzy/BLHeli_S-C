#ifndef BLHELI_S_ESC_TYPES_H
#define BLHELI_S_ESC_TYPES_H

#include <stdbool.h>
#include <stdint.h>

enum blheli_s_control_phase {
    BLHELI_S_CONTROL_STOPPED,
    BLHELI_S_CONTROL_STARTUP,
    BLHELI_S_CONTROL_INITIAL_RUN,
    BLHELI_S_CONTROL_RUNNING,
    BLHELI_S_CONTROL_BRAKING
};

#endif
