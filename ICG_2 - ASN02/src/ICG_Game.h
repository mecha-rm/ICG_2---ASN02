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