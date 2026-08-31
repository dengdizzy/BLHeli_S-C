#include "core/esc_control.h"

void blheli_s_core_init(struct blheli_s_core_state *state)
{
    blheli_s_core_reset(state);
    state->initialized = true;
}

void blheli_s_core_reset(struct blheli_s_core_state *state)
{
    *state = (struct blheli_s_core_state){
        .phase = BLHELI_S_CONTROL_STOPPED,
        .initialized = false,
        .reserved = 0u
    };
}
