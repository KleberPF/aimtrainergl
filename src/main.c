#include <stdio.h>

#include "game.h"

int main()
{
	Game* game = game_create();

	game_main_loop(game);

	game_destroy(game);

	return 0;
}
