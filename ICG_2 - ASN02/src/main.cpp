/*
 * Name: Bonus Fruit
	- Kennedy Adams	(100632983)
	- Jonah Griffin	(100702748)
	- Nathan Tuck	(100708651)
	- Stephane Gagnon (100694227)
	- Roderick "R.J." Montague (100701758)
	* do note that only those in Intermediate Computer Graphics are listed.
 * Date: 03/03/2020
 * Description: Intermdiate Computer Graphics - Assignment 2
 * References:
*/

// Bonus Fruit - Cherry Engine
#include "ICG_Game.h"
#include <toolkit/Logging.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main() 
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // checks for memory leaks once the program ends.
	long long allocPoint = 0;
	if (allocPoint)
	{
		_CrtSetBreakAlloc(allocPoint); // sets where you want to stop our program by assigning the allocation block index stopping point.
	}

	Logger::Init();

	// game object
	icg::ICG_Game* game = new icg::ICG_Game("ICG_2 - ASN02", 850, 850, false);
	cherry::Game::FPS = 0; // no frame cap
	game->Run();
	delete game;

	Logger::Uninitialize();

	return 0;
}