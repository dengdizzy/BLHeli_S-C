#include <assert.h>

#include "core/throttle_calibration.h"

void test_throttle_calibration(void)
{
    struct blheli_s_throttle_gain gain =
        blheli_s_find_throttle_gain(37u, 208u, 0u);
    struct blheli_s_throttle_gains bidirectional =
        blheli_s_find_throttle_gains(37u, 122u, 208u, true, false);

    assert(gain.gain == 1u && gain.multiplier == 0u);
    assert(blheli_s_find_throttle_gain(0u, 0u, 0u).gain == 4u);
    assert(bidirectional.forward.gain == 1u);
    assert(bidirectional.reverse.gain == 1u);
}
