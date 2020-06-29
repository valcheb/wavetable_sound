#include "raw_player.h"

static uint32_t song_len = 0;
static uint32_t i = 0;

void raw_init()
{
    song_len = SONG_LEN;
}

bool raw_is_continue()
{
    if (song_len > 0)
        return true;
    return false;
}

uint8_t raw_get_value()
{
    song_len--;
    return raw[i++];
}
