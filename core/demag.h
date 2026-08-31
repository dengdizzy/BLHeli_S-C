#ifndef BLHELI_S_DEMAG_H
#define BLHELI_S_DEMAG_H

#include <stdbool.h>
#include <stdint.h>

struct blheli_s_demag_state {
    uint8_t metric;
    uint8_t power_off_threshold;
    bool detected;
    bool power_cut;
};

void blheli_s_demag_begin(struct blheli_s_demag_state *state,
                          uint8_t power_off_threshold);
void blheli_s_demag_update(struct blheli_s_demag_state *state,
                           bool detected);

#endif
