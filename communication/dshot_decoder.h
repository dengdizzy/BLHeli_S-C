#ifndef BLHELI_S_DSHOT_DECODER_H
#define BLHELI_S_DSHOT_DECODER_H

#include <stdbool.h>
#include <stdint.h>

struct blheli_s_dshot_packet {
    uint16_t throttle;
    uint8_t command;
    bool telemetry;
    bool is_command;
};

bool blheli_s_dshot_decode(uint16_t frame,
                           struct blheli_s_dshot_packet *packet);

#endif
