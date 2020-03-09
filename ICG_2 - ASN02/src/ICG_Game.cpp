/*
 * Name: Bonus Fruit
	- Kennedy Adams	(100632983)
	- Jonah Griffin	(100702748)
	- Nathan Tuck	(100708651)
	- Stephane Gagnon (100694227)
	- Roderick "R.J." Montague (100701758)
 * Date: 03/08/2020
 * Description: game class (makes opening scene)
 * References:
	* http://www.cplusplus.com/reference/vector/vector/resize/
*/

#include "ICG_Game.h"
#include "ICG_MainScene.h"

// constructor
icg::ICG_Game::ICG_Game(const char windowTitle[WINDOW_TITLE_CHAR_MAX], float _width, float _height, bool _fullScreen)
	: Game(windowTitle, _width, _height, _fullScreen, nullptr, true) 
{
}

// loads the content.
void icg::ICG_Game::LoadContent()
{
	Game::LoadContent();

	// main scene.
	ICG_MainScene * mainScene = new ICG_MainScene("ICG_2 - ASN02");
	RegisterScene(mainScene, true);
}

// drawing imgui
void icg::ICG_Game::DrawGui(float deltaTime)
{
	cherry::Game::DrawGui(deltaTime);
}

// update loop.
void icg::ICG_Game::Update(float deltaTime)
{
	Game::Update(deltaTime);
}
