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

void ma_init_systick()
{
    f030_init_systick();
}

void ma_systick_start_measure()
{
    f030_systick_start_measure();
}

uint32_t ma_systick_stop_measure()
{
    return f030_systick_stop_measure();
}
