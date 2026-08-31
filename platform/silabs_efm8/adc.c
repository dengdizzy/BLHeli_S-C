#include "platform/silabs_efm8/esc_hal.h"

void silabs_efm8_configure_adc(struct silabs_efm8_registers *registers)
{
    registers->adc_config = 0x59u;
    registers->adc_control = 0x80u;
}
