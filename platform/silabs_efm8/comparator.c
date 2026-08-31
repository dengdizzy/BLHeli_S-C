#include "platform/silabs_efm8/esc_hal.h"

void silabs_efm8_configure_comparator(
    struct silabs_efm8_registers *registers)
{
    registers->comparator_control = 0x4au;
}
