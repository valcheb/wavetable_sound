#include "mcu_adapter.h"
#include "f030.h"

void ma_init_mcu()
{
    f030_init_mcu();
}

void ma_pwm_play(uint8_t data)
{
    f030_pwm_play(data);
}

void ma_set_expand(uint8_t value)
{
    f030_set_expand(value);
}

bool ma_is_data_needed()
{
    return f030_is_data_needed();
}

void ma_enable_pwm()
{
    f030_enable_pwm();
}

void ma_disable_pwm()
{
    f030_disable_pwm();
}
