#include "communication/dshot_decoder.h"

bool blheli_s_dshot_decode(uint16_t frame,
                           struct blheli_s_dshot_packet *packet)
{
    uint16_t value = frame >> 4;
    uint8_t checksum = (uint8_t)(frame & 0x0fu);
    uint8_t calculated = (uint8_t)((value ^ (value >> 4) ^
                                    (value >> 8)) & 0x0fu);
    uint16_t throttle;

    if (checksum != calculated) {
        return false;
    }
    throttle = (uint16_t)((frame >> 5) & 0x07ffu);
    packet->telemetry = (frame & 0x10u) != 0u;
    packet->is_command = throttle < 48u && throttle != 0u;
    packet->command = packet->is_command ? (uint8_t)(throttle / 2u) : 0u;
    packet->throttle = packet->is_command ? 0u : throttle;
    return true;
}
