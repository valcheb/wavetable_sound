#include "player.h"
#include "wavetable_synth.h"

void pl_init()
{
    ma_init_mcu();
    wts_init();
}

void pl_start_play()
{
    ma_enable_pwm();
}

void pl_stop_play()
{
    ma_pwm_play(0x00);
    ma_disable_pwm();
}

void pl_play_song() //TODO rewrite with argument but reduce global variables
{
    if (!wts_is_full())
    {
        wts_cook_data();
    }

    if (ma_is_data_needed() && !wts_is_empty())
    {
        ma_pwm_play(wts_get_value());
    }
}

bool pl_is_continue()
{
    return wts_is_continue();
}
