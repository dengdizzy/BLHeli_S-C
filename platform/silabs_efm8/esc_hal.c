#include "platform/silabs_efm8/esc_hal.h"

void silabs_efm8_configure_timers(struct silabs_efm8_registers *);
void silabs_efm8_configure_pwm(struct silabs_efm8_registers *);
void silabs_efm8_configure_comparator(struct silabs_efm8_registers *);
void silabs_efm8_configure_adc(struct silabs_efm8_registers *);

bool silabs_efm8_initialize(struct silabs_efm8_registers *registers)
{
    if (registers == 0) {
        return false;
    }
    *registers = (struct silabs_efm8_registers){0};
    registers->interrupts_enabled = false;
    silabs_efm8_configure_timers(registers);
    silabs_efm8_configure_pwm(registers);
    silabs_efm8_configure_comparator(registers);
    silabs_efm8_configure_adc(registers);
    registers->interrupts_enabled = true;
    return true;
}
