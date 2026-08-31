#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "communication/dshot_decoder.h"
#include "core/commutation.h"
#include "core/protection.h"
#include "core/timing_control.h"

static uint16_t dshot_frame(uint16_t throttle)
{
    uint16_t value = (uint16_t)(throttle << 1);
    uint8_t checksum = (uint8_t)((value ^ (value >> 4) ^
                                  (value >> 8)) & 0x0fu);
    return (uint16_t)((value << 4) | checksum);
}

void test_trace_replay(void)
{
    static const enum blheli_s_commutation_step sequence[] = {
        BLHELI_S_COMMUTATION_STEP_1, BLHELI_S_COMMUTATION_STEP_2,
        BLHELI_S_COMMUTATION_STEP_3, BLHELI_S_COMMUTATION_STEP_4,
        BLHELI_S_COMMUTATION_STEP_5, BLHELI_S_COMMUTATION_STEP_6,
        BLHELI_S_COMMUTATION_STEP_1
    };
    struct blheli_s_dshot_packet packet;
    struct blheli_s_timing_waits waits;
    enum blheli_s_commutation_step step = sequence[0];

    for (size_t i = 1u; i < sizeof(sequence) / sizeof(sequence[0]); ++i) {
        step = blheli_s_commutation_next_step(step);
        assert(step == sequence[i]);
    }

    assert(blheli_s_dshot_decode(dshot_frame(1000u), &packet));
    assert(packet.throttle == 1000u);
    blheli_s_calculate_timing_waits(400u, 3u, false, false, &waits);
    assert(waits.commutation == 200u);
    assert(waits.advance == 200u);
    assert(waits.zero_cross_scan == 100u);
    assert(waits.zero_cross_timeout == 200u);
    assert(blheli_s_temperature_pwm_limit(157u, 150u) == 128u);
    assert(blheli_s_high_rpm_pwm_limit(300u, 1u, false) == 0u);
}
