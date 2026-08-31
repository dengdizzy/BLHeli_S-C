#include <assert.h>

#include "hal/esc_hal_adc.h"
#include "hal/esc_hal_input.h"
#include "hal/esc_hal_motor.h"
#include "hal/esc_hal_platform.h"
#include "hal/esc_hal_storage.h"

void test_hal_interface(void)
{
    struct esc_hal_input_capture capture = { 0u, 0u };
    uint16_t adc_value = 0u;

    assert(capture.timestamp == 0u);
    assert(capture.width == 0u);
    assert(adc_value == 0u);
}
