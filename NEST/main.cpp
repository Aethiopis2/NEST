/**
 * @brief driver module for NEST
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */


 //=====================================================================|
#include "game.hpp"



//=====================================================================|
int main(int argc, char* argv[])
{
	Game game;

	if (!game.Init("NEST v2.0"))
		return 1;

	while (game.Is_Running())
	{
		if (game.Is_Paused())
			continue;

		game.Render();
		game.Update();
		game.Handle_Events();
	} // end while 

	return 0;
} // end main