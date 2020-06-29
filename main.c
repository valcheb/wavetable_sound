#include "player.h"

int main()
{
	pl_init();
	pl_start_play();

	while(pl_is_continue())
	{
		pl_play_song();
	}

	pl_stop_play();

	return 0;
}
