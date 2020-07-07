#include "mcu_adapter.h"
#include "f407.h"

void ma_init_mcu()
{
    f407_init_mcu();
}

void ma_pwm_play(uint8_t data)
{
    f407_pwm_play(data);
}

bool ma_is_data_needed()
{
    return f407_is_data_needed();
}

void ma_enable_pwm()
{
    f407_enable_pwm();
}

void ma_disable_pwm()
{
    f407_disable_pwm();
}
