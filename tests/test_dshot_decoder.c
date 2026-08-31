#include <assert.h>

#include "communication/dshot_decoder.h"

static uint16_t make_frame(uint16_t throttle, bool telemetry)
{
    uint16_t value = (uint16_t)((throttle << 1) | (telemetry ? 1u : 0u));
    uint8_t checksum = (uint8_t)((value ^ (value >> 4) ^
                                  (value >> 8)) & 0x0fu);
    return (uint16_t)((value << 4) | checksum);
}

void test_dshot_decoder(void)
{
    struct blheli_s_dshot_packet packet;

    assert(blheli_s_dshot_decode(make_frame(1000u, false), &packet));
    assert(packet.throttle == 1000u);
    assert(!packet.is_command);
    assert(!packet.telemetry);
    assert(blheli_s_dshot_decode(make_frame(12u, true), &packet));
    assert(packet.is_command);
    assert(packet.command == 6u);
    assert(packet.telemetry);
    assert(!blheli_s_dshot_decode(
        (uint16_t)(make_frame(1000u, false) ^ 1u), &packet));
}
