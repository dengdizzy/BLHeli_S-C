#include "core/timing_control.h"

void blheli_s_calculate_timing_waits(
    uint16_t commutation_period_four_x, uint8_t commutation_timing,
    bool clock_48mhz, bool startup, struct blheli_s_timing_waits *waits)
{
    uint16_t period = commutation_period_four_x;
    uint16_t quarter;
    uint16_t advance;

    if (clock_48mhz) {
        period >>= 1u;
    }
    quarter = period >> 1u;
    advance = quarter;
    if (commutation_timing > 3u) {
        if ((commutation_timing & 1u) == 0u) {
            advance = (uint16_t)(advance + (quarter >> 1u));
        } else {
            advance = (uint16_t)(advance + quarter);
        }
    } else if (commutation_timing < 3u) {
        advance = (uint16_t)(advance - (quarter >> 1u));
    }

    *waits = (struct blheli_s_timing_waits){
        .commutation = startup ? 0xfff0u : quarter,
        .advance = advance,
        .zero_cross_scan = startup ? 0xfff0u : (quarter >> 1u),
        .zero_cross_timeout = startup ? 0xfff0u : quarter
    };
}
