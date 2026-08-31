#include <assert.h>

#include "core/timing_control.h"

void test_timing_control(void)
{
    struct blheli_s_timing_waits waits;

    blheli_s_calculate_timing_waits(400u, 3u, false, false, &waits);
    assert(waits.commutation == 200u);
    assert(waits.advance == 200u);
    assert(waits.zero_cross_scan == 100u);
    assert(waits.zero_cross_timeout == 200u);

    blheli_s_calculate_timing_waits(400u, 4u, false, false, &waits);
    assert(waits.advance == 300u);
    blheli_s_calculate_timing_waits(400u, 3u, false, true, &waits);
    assert(waits.commutation == 0xfff0u);
    assert(waits.zero_cross_scan == 0xfff0u);
}
