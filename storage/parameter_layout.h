#ifndef BLHELI_S_PARAMETER_LAYOUT_H
#define BLHELI_S_PARAMETER_LAYOUT_H

#include <stdint.h>

#define BLHELI_S_PARAMETER_LAYOUT_REVISION UINT8_C(33)
#define BLHELI_S_PARAMETER_SIGNATURE_LOW UINT8_C(0x55)
#define BLHELI_S_PARAMETER_SIGNATURE_HIGH UINT8_C(0xaa)
#define BLHELI_S_PARAMETER_NAME_LENGTH UINT8_C(16)

struct blheli_s_parameter_header {
    uint8_t firmware_main_revision;
    uint8_t firmware_sub_revision;
    uint8_t layout_revision;
    uint8_t initialized_signature_low;
    uint8_t initialized_signature_high;
};

#endif
