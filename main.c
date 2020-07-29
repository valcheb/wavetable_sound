#include "player.h"
#include "song.h"

int main()
{
	ma_init_mcu();

	pl_init(song);
	pl_start_play();

	while(pl_is_continue())
	{
		pl_play_song();
	}

	pl_stop_play();

	return 0;
}
