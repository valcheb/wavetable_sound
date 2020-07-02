#include "player.h"

//#define PLAY_RAW

#ifdef PLAY_RAW
# include "raw_player.h"
#else
# include "wavetable_synth.h"
#endif

void pl_init()
{
    ma_init_mcu();
#ifdef PLAY_RAW //вместо кучи ifdef можно использовать структуру с указателями на функции
    raw_init();
    ma_set_expand(6); //mcu_rate/song_rate
#else
    wts_init();
    ma_set_expand(1);
#endif
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
