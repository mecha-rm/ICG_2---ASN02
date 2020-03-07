#pragma once
#include "cherry/scenes/GameplayScene.h"
#include "PostLight.h"
#include <vector>

#define BLINN_PHONG_POST "res/shaders/post/blinn-phong-post.fs.glsl"
#define BLINN_PHONG_POST_MULTI "res/shaders/post/blinn-phong-post-multi.fs.glsl"

namespace icg
{
	class ICG_MainScene : public cherry::GameplayScene
	{
	public:
		// main scene
		ICG_MainScene(const std::string a_Name);



		// scene opening
		void OnOpen() override;

		// scene closing
		void OnClose() override;

		// these functions get called by the game class by default, but they can be overwritten.
		void MouseButtonPressed(GLFWwindow* window, int button) override;

		// called when a mouse button is being held.
		void MouseButtonHeld(GLFWwindow* window, int button) override;

		// called when a mouse button has been pressed
		void MouseButtonReleased(GLFWwindow* window, int button) override;


		// called when a key has been pressed
		void KeyPressed(GLFWwindow* window, int key) override;

		// called when a key is being held down
		void KeyHeld(GLFWwindow* window, int key) override;

		// called when a key has been released
		void KeyReleased(GLFWwindow* window, int key) override;

		void Update(float deltaTime) override;
	
	private:
		// loads the lights from a file
		void LoadFromFile(std::string filePath);

		// if 'true', then the clear colour gets used.
		void UseClearColor(bool useClear);

		// translation direction
		glm::vec3 t_Dir = glm::vec3(0, 0, 0);

		// rotation direction
		glm::vec3 r_Dir = glm::vec3(0, 0, 0);

		// translation and rotation speeds
		float t_Inc = 60.5F;
		float r_Inc = 75.0F;

		// the left, middle, and right mouse buttons
		bool mbLeft = false, mbMiddle = false, mbRight = false;

		// Assignment Variables

		// Multi-lights
		// cherry::Shader::Sptr ls_shader; // shader
		cherry::FrameBuffer::Sptr ls_fb; // frame buffer

		// vector of lights
		std::vector<PostLight*> lights;

		// single post light
		PostLight * postLight = nullptr;

		// if 'true', the default light is generated.
		const bool DEFAULT_LIGHT_ENABLED = true;
		
	protected:

	};
}