#ifndef BLHELI_S_ESC_STATE_H
#define BLHELI_S_ESC_STATE_H

#include "core/esc_types.h"

struct blheli_s_core_state {
    enum blheli_s_control_phase phase;
    bool initialized;
    uint8_t reserved;
};

#endif
