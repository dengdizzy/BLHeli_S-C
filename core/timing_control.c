#include "core/timing_control.h"

static uint16_t clamp_wait(uint16_t value, bool *minimum_wait_clamped)
{
    if (value == 0u) {
        *minimum_wait_clamped = true;
        return 1u;
    }
    return value;
}

uint8_t blheli_s_timing_effective_commutation_timing(
    uint8_t programmed_commutation_timing, uint8_t demag_metric,
    bool startup_phase)
{
    uint8_t timing = startup_phase ? 3u : programmed_commutation_timing;

    if (!startup_phase) {
        if (demag_metric >= 130u) {
            timing++;
        }
        if (demag_metric >= 160u) {
            timing++;
        }
        if (timing > 5u) {
            timing = 5u;
        }
    }

    return timing;
}

void blheli_s_calculate_timing_waits(
    uint16_t commutation_period_four_x, uint8_t commutation_timing,
    bool clock_48mhz, bool startup, struct blheli_s_timing_waits *waits)
{
    uint16_t period = commutation_period_four_x;
    uint16_t quarter;
    uint16_t advance;
    bool minimum_wait_clamped = false;

    if (clock_48mhz) {
        period >>= 1u;
    }
    quarter = clamp_wait((uint16_t)(period >> 1u), &minimum_wait_clamped);
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
        .advance = clamp_wait(advance, &minimum_wait_clamped),
        .zero_cross_scan = startup ? 0xfff0u :
            clamp_wait((uint16_t)(quarter >> 1u), &minimum_wait_clamped),
        .zero_cross_timeout = startup ? 0xfff0u : quarter
    };
}

void blheli_s_timing_calculate_next(
    const struct blheli_s_timing_descriptor *descriptor,
    struct blheli_s_timing_update *update)
{
    uint32_t current = descriptor->current_commutation;
    uint32_t previous = descriptor->previous_commutation;
    uint32_t previous_previous = descriptor->previous_previous_commutation;
    uint16_t previous_period = descriptor->commutation_period_four_x;
    uint16_t new_delta;
    uint32_t new_period;
    bool fast_path = descriptor->high_rpm ||
        ((descriptor->commutation_period_four_x >> 8u) < 2u);
    bool overflow_clamped = false;
    bool minimum_wait_clamped = false;
    struct blheli_s_timing_waits waits;
    uint8_t effective_timing =
        blheli_s_timing_effective_commutation_timing(
            descriptor->programmed_commutation_timing,
            descriptor->demag_metric, descriptor->startup_phase);

    if (descriptor->clock_48mhz) {
        current >>= 1u;
        previous >>= 1u;
        previous_previous >>= 1u;
    }

    if (descriptor->startup_phase) {
        if ((current >> 16u) != (previous >> 16u)) {
            new_delta = 0xffffu;
            overflow_clamped = true;
        } else {
            new_delta = (uint16_t)(current - previous_previous);
        }
        new_period = (previous_period >> 1u) + new_delta;
    } else if (fast_path) {
        new_delta = (uint16_t)(current - previous);
        new_period = (uint32_t)previous_period - (previous_period >> 4u) +
                     (new_delta >> 2u);
    } else {
        uint8_t period_divisor = 4u;
        uint8_t new_divisor = 2u;
        uint8_t previous_period_high = (uint8_t)(previous_period >> 8u);

        new_delta = (uint16_t)(current - previous);
        if (previous_period_high >= 4u) {
            period_divisor--;
            new_divisor--;
        }
        if (previous_period_high >= 8u && !descriptor->initial_run_phase) {
            period_divisor--;
            new_divisor--;
        }
        new_period = (uint32_t)previous_period -
                     (previous_period >> period_divisor) +
                     (new_divisor == 0u ? new_delta :
                                          (new_delta >> new_divisor));
    }

    if (new_period > 0xffffu) {
        new_period = 0xffffu;
        overflow_clamped = true;
    }

    blheli_s_calculate_timing_waits((uint16_t)new_period, effective_timing,
                                    descriptor->clock_48mhz,
                                    descriptor->startup_phase, &waits);
    minimum_wait_clamped =
        (!descriptor->startup_phase &&
         (waits.commutation == 1u ||
          waits.advance == 1u ||
          waits.zero_cross_scan == 1u ||
          waits.zero_cross_timeout == 1u));

    *update = (struct blheli_s_timing_update){
        .previous_commutation = current,
        .previous_previous_commutation = previous,
        .commutation_period_four_x = (uint16_t)new_period,
        .waits = waits,
        .effective_commutation_timing = effective_timing,
        .high_rpm = ((new_period >> 8u) < 2u),
        .fast_path = fast_path,
        .overflow_clamped = overflow_clamped,
        .minimum_wait_clamped = minimum_wait_clamped,
        .timer_action = BLHELI_S_TIMING_TIMER_ARM_ZERO_CROSS_SCAN
    };
}
