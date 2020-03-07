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
