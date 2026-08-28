#include <assert.h>
#include <stddef.h>

#include "core/esc_control.h"
#include "storage/parameter_layout.h"

void test_state_layout(void)
{
    struct blheli_s_core_state state;
    struct blheli_s_parameter_header header = { 0u, 0u, 0u, 0u, 0u };

    blheli_s_core_reset(&state);
    assert(state.motor.startup_count == 0u);
    assert(state.timing.commutation_period_four_x == 0u);
    assert(state.input.format == BLHELI_S_INPUT_FORMAT_UNKNOWN);
    assert(state.protection.demag_detected_metric == 0u);
    assert(offsetof(struct blheli_s_core_state, motor) >
           offsetof(struct blheli_s_core_state, reserved));
    assert(sizeof(header) == 5u);
    assert(BLHELI_S_PARAMETER_LAYOUT_REVISION == 33u);
    assert(BLHELI_S_PARAMETER_SIGNATURE_LOW == 0x55u);
    assert(BLHELI_S_PARAMETER_SIGNATURE_HIGH == 0xaau);
    assert(BLHELI_S_PARAMETER_NAME_LENGTH == 16u);
}
