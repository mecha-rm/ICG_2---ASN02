#pragma once

#include "cherry/Game.h"

namespace icg
{
	class ICG_Game : public cherry::Game
	{
	public:
		ICG_Game(const char windowTitle[WINDOW_TITLE_CHAR_MAX], float _width, float _height, bool _fullScreen);

		// load content
		void LoadContent() override;

		// draw imgui
		void DrawGui(float deltaTime) override;

		// update loop.
		void Update(float deltaTime) override;

	private:
	
	protected:

	};
}