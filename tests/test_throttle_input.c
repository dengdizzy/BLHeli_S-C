#include <assert.h>

#include "core/throttle_input.h"

void test_throttle_input(void)
{
    const struct blheli_s_throttle_input_config config = {
        .minimum = 1000u,
        .center = 1500u,
        .maximum = 2000u,
        .forward_gain = { 128u, 0u },
        .reverse_gain = { 128u, 0u },
        .bidirectional = true,
        .full_range = false,
        .bidirectional_reversed = false
    };
    struct blheli_s_throttle_input_result result;

    result = blheli_s_process_throttle(1600u, &config);
    assert(result.value == 90u);
    assert(!result.direction_reversed);
    result = blheli_s_process_throttle(1400u, &config);
    assert(result.value == 390u);
    assert(result.direction_reversed);
    result = blheli_s_process_throttle(1505u, &config);
    assert(result.stop);
    result = blheli_s_process_throttle(2300u, &config);
    assert(result.outside_range);
}
