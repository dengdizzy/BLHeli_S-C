#include <assert.h>

#include "core/timing_control.h"

void test_timing_control(void)
{
    struct blheli_s_timing_descriptor descriptor = {
        .current_commutation = 1000u,
        .previous_commutation = 600u,
        .previous_previous_commutation = 200u,
        .commutation_period_four_x = 0x0200u,
        .programmed_commutation_timing = 3u,
        .demag_metric = 0u,
        .clock_48mhz = false,
        .startup_phase = false,
        .initial_run_phase = false,
        .high_rpm = false
    };
    struct blheli_s_timing_update update;
    struct blheli_s_timing_waits waits;

    blheli_s_calculate_timing_waits(400u, 3u, false, false, &waits);
    assert(waits.commutation == 200u);
    assert(waits.advance == 200u);
    assert(waits.zero_cross_scan == 100u);
    assert(waits.zero_cross_timeout == 200u);

    blheli_s_calculate_timing_waits(400u, 4u, false, false, &waits);
    assert(waits.advance == 300u);
    blheli_s_calculate_timing_waits(400u, 5u, false, false, &waits);
    assert(waits.advance == 400u);
    blheli_s_calculate_timing_waits(400u, 2u, false, false, &waits);
    assert(waits.advance == 100u);
    blheli_s_calculate_timing_waits(400u, 3u, true, false, &waits);
    assert(waits.commutation == 100u);
    assert(waits.zero_cross_scan == 50u);
    blheli_s_calculate_timing_waits(400u, 3u, false, true, &waits);
    assert(waits.commutation == 0xfff0u);
    assert(waits.zero_cross_scan == 0xfff0u);

    assert(blheli_s_timing_effective_commutation_timing(3u, 0u, false) ==
           3u);
    assert(blheli_s_timing_effective_commutation_timing(3u, 130u, false) ==
           4u);
    assert(blheli_s_timing_effective_commutation_timing(3u, 160u, false) ==
           5u);
    assert(blheli_s_timing_effective_commutation_timing(5u, 160u, false) ==
           5u);
    assert(blheli_s_timing_effective_commutation_timing(5u, 160u, true) ==
           3u);

    blheli_s_timing_calculate_next(&descriptor, &update);
    assert(update.commutation_period_four_x == 580u);
    assert(update.waits.commutation == 290u);
    assert(update.waits.zero_cross_scan == 145u);
    assert(update.waits.zero_cross_timeout == 290u);
    assert(update.effective_commutation_timing == 3u);
    assert(!update.fast_path);
    assert(!update.high_rpm);
    assert(!update.overflow_clamped);
    assert(update.timer_action == BLHELI_S_TIMING_TIMER_ARM_ZERO_CROSS_SCAN);

    descriptor.demag_metric = 130u;
    blheli_s_timing_calculate_next(&descriptor, &update);
    assert(update.effective_commutation_timing == 4u);
    assert(update.waits.advance == 435u);
    descriptor.demag_metric = 0u;

    descriptor.commutation_period_four_x = 0x0800u;
    descriptor.initial_run_phase = true;
    blheli_s_timing_calculate_next(&descriptor, &update);
    assert(update.commutation_period_four_x == 1992u);
    descriptor.initial_run_phase = false;
    blheli_s_timing_calculate_next(&descriptor, &update);
    assert(update.commutation_period_four_x == 1936u);

    descriptor.commutation_period_four_x = 0x0100u;
    blheli_s_timing_calculate_next(&descriptor, &update);
    assert(update.commutation_period_four_x == 340u);
    assert(update.fast_path);
    assert(update.high_rpm);

    descriptor.commutation_period_four_x = 0x0200u;
    descriptor.clock_48mhz = true;
    descriptor.current_commutation = 2000u;
    descriptor.previous_commutation = 1200u;
    blheli_s_timing_calculate_next(&descriptor, &update);
    assert(update.commutation_period_four_x == 580u);
    assert(update.waits.commutation == 145u);

    descriptor.clock_48mhz = false;
    descriptor.startup_phase = true;
    descriptor.commutation_period_four_x = 0xf000u;
    descriptor.current_commutation = 3000u;
    descriptor.previous_commutation = 2000u;
    descriptor.previous_previous_commutation = 1000u;
    blheli_s_timing_calculate_next(&descriptor, &update);
    assert(update.commutation_period_four_x == 32720u);
    assert(update.waits.commutation == 0xfff0u);
    assert(update.waits.advance == 16360u);
    assert(update.waits.zero_cross_scan == 0xfff0u);
    assert(update.effective_commutation_timing == 3u);
    assert(!update.overflow_clamped);

    descriptor.current_commutation = 0x00020000u;
    descriptor.previous_commutation = 0u;
    descriptor.previous_previous_commutation = 0u;
    blheli_s_timing_calculate_next(&descriptor, &update);
    assert(update.commutation_period_four_x == 0xffffu);
    assert(update.overflow_clamped);

    descriptor.startup_phase = false;
    descriptor.current_commutation = 100u;
    descriptor.previous_commutation = 100u;
    descriptor.commutation_period_four_x = 2u;
    blheli_s_timing_calculate_next(&descriptor, &update);
    assert(update.waits.commutation == 1u);
    assert(update.waits.zero_cross_scan == 1u);
    assert(update.minimum_wait_clamped);
}
