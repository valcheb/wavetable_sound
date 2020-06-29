#ifndef PLAYER_H_
#define PLAYER_H_

#include "mcu_adapter.h"

void pl_init();
void pl_play_song();
void pl_start_play();
void pl_stop_play();
bool pl_is_continue();

#endif /*PLAYER_H_*/
