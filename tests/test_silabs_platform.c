#include <assert.h>

#include "platform/silabs_efm8/esc_hal.h"

void test_silabs_platform(void)
{
    struct silabs_efm8_registers registers;

    assert(silabs_efm8_initialize(&registers));
    assert(registers.timer0_mode == 0x01u);
    assert(registers.timer2_control == 0x04u);
    assert(registers.pca_control == 0x40u);
    assert(registers.pca_pwm_mode == 0x82u);
    assert(registers.pca_polarity == 0x00u);
    assert(registers.comparator_control == 0x4au);
    assert(registers.adc_config == 0x59u);
    assert(registers.adc_control == 0x80u);
    assert(registers.interrupts_enabled);
    assert(!silabs_efm8_initialize(0));
}
