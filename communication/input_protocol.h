#ifndef BLHELI_S_INPUT_PROTOCOL_H
#define BLHELI_S_INPUT_PROTOCOL_H

#include <stdint.h>

struct blheli_s_input_event {
    uint8_t throttle;
    uint8_t command;
};

#endif
