/**
 * @brief driver module for NEST acrnoym for Nintendo Entertainment System emulaTor
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */


//=====================================================================|
#include "NEST.hpp"



//=====================================================================|
int main(int argc, char* argv[])
{
	NEST NEST;

	if (!NEST.Init("NEST"))
		return 1;

	while (NEST.Is_Running())
	{
		if (NEST.Is_Paused())
			continue;

		NEST.Render();
		NEST.Update();
		NEST.Handle_Events();
	} // end while 

	return 0;
} // end main