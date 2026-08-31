#include "core/demag.h"

void blheli_s_demag_begin(struct blheli_s_demag_state *state,
                          uint8_t power_off_threshold)
{
    *state = (struct blheli_s_demag_state){
        .metric = 120u,
        .power_off_threshold = power_off_threshold,
        .detected = false,
        .power_cut = false
    };
}

void blheli_s_demag_update(struct blheli_s_demag_state *state,
                           bool detected)
{
    uint16_t metric = (uint16_t)state->metric * 7u;

    state->detected = detected;
    if (detected) {
        metric = (uint16_t)(metric + 256u);
    }
    state->metric = (uint8_t)(metric >> 3u);
    if (state->metric < 120u) {
        state->metric = 120u;
    }
    state->power_cut = state->metric >= state->power_off_threshold;
}
