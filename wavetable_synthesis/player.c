#include "player.h"

//#define PLAY_RAW

#ifdef PLAY_RAW
# include "raw_player.h"
#else
# include "wavetable_synth.h"
#endif

void pl_init()
{
    ma_init_mcu(); //здесь должен быть учет частоты дискретизации и расширения частоты для raw
#ifdef PLAY_RAW //вместо кучи ifdef можно использовать структуру с указателями на функции
    raw_init();
#else
    wts_init();
#endif
}

void pl_start_play()
{
    ma_enable_pwm();
}

void pl_stop_play()
{
    ma_disable_pwm();
}

void pl_play_song()
{
#ifdef PLAY_RAW
    if (ma_is_data_needed())
    {
        ma_pwm_play(raw_get_value());
    }
#else
    if (!wts_is_full()) //is_cook_continue && !is_full
    {
        wts_cook_data();  //буфер заполняется COOK_SIZE лишних раз. на воспроизведение raw не влияет
    }

    if (ma_is_data_needed() && !wts_is_empty())
    {
            ma_pwm_play(wts_get_value());
    }
#endif
}

bool pl_is_continue()
{
#ifdef PLAY_RAW
    return raw_is_continue();
#else
    return wts_is_continue();
#endif
}
/*

player_init : wts_init - raw_init
player_start : ma_enable
player_stop : ma_disable
player_play : play_syth - play_raw

*/
