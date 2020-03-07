#pragma once
#include "cherry/scenes/GameplayScene.h"
// #include "PostLight.h"
#include <vector>

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
		cherry::Shader::Sptr l_shader; // shader
		cherry::FrameBuffer::Sptr l_fb; // frame buffer

		// a path for the light to follow.
		cherry::Path path;

		// post processing light.
		struct PostLight
		{
			glm::vec3 position;
			glm::vec3 color;
			float attenuation;
			float shininess;
		};

		// the post processed light.
		PostLight postLightOrig; // starting values
		PostLight postLight; // current point light.

		// the sphere representing the volume of the light.
		cherry::PrimitiveUVSphere* lightBody = nullptr;

		std::vector<PostLight> lights;
	protected:

	};
}