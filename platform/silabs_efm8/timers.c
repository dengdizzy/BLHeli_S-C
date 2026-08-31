#include "platform/silabs_efm8/esc_hal.h"

static void configure_timers(struct silabs_efm8_registers *registers)
{
    registers->timer0_mode = 0x01u;
    registers->timer1_mode = 0x00u;
    registers->timer2_control = 0x04u;
    registers->timer3_control = 0x00u;
}

void silabs_efm8_configure_timers(struct silabs_efm8_registers *registers)
{
    configure_timers(registers);
}
