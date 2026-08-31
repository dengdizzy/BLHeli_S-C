#include "platform/silabs_efm8/esc_hal.h"

void silabs_efm8_configure_pwm(struct silabs_efm8_registers *registers)
{
    registers->pca_control = 0x40u;
    registers->pca_pwm_mode = 0x82u;
    registers->pca_polarity = 0x00u;
}
