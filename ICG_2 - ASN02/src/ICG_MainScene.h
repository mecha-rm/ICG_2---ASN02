/*
 * Name: Bonus Fruit
	- Kennedy Adams	(100632983)
	- Jonah Griffin	(100702748)
	- Nathan Tuck	(100708651)
	- Stephane Gagnon (100694227)
	- Roderick "R.J." Montague (100701758)
 * Date: 03/08/2020
 * Description: main scene
 * References:
	* http://www.cplusplus.com/reference/vector/vector/resize/
*/

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

		// imgui draw
		void DrawGui(float deltaTime) override;

		// activates a light
		void EnableLight();

		// deactivates a light
		void DisableLight();

		// sets the amount of enabled lights.
		void SetEnabledLights(int enabled);
 
		// returns 'true' if light volumes are visible.
		bool GetLightVolumesVisible() const;

		// sets whether the light volumes are visible or not.
		void SetLightVolumesVisible(bool visible);

		// cycles light volume types
		void CycleLightVolumes();

		// enables the position/depth buffer view
		void EnablePositionView();

		// enables the normal view
		void EnableNormalView();

		// shows the materials of the objects.
		void EnableMaterialView();

		// shows the lighting effects
		void EnableLightingView();

		// enables the first effect
		void EnableEffect1();

		// enables the second effect
		void EnableEffect2();

		// update
		void Update(float deltaTime) override;
	
		// if 'true', the default light is generated.
		static const bool DEFAULT_LIGHT_ENABLED;

	private:
		// loads the lights from a file
		void LoadFromFile(std::string filePath);

		// loads the objects for the scene.
		void LoadObjects();

		// activates (or deactivates) a light.
		void LightActivation(bool activate);

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
		
		// the amount of enabled lights
		int enabledLights = 0;

		// key related variables
		bool key1 = false; // turns off all but the first light.

		// saves whether the volumes are currently visible or not.
		bool volumesVisible = true;

		// the imgui's light index
		int imguiLightIndex = 0;

		// used to get the coordinate points for a light
		glm::vec3 imguiCoord;

		// the layer for lighting
		cherry::PostLayer* lightLayer = nullptr;

		// a layer used for showing the depth buffer
		cherry::PostLayer* depthLayer = nullptr;

		// a layer used for showing the normal layer
		cherry::PostLayer* normalLayer = nullptr;

		// the two effect layers
		cherry::PostLayer* effectLayer1 = nullptr;
		cherry::PostLayer* effectLayer2 = nullptr;

		// struct for object rotation.
		typedef struct ObjectRotationBehaviour
		{
			cherry::Object* object;
			glm::vec3 rInc;
		} ObjRotate;

		// rotates designated objects.
		std::vector<ObjectRotationBehaviour> objRots;

		// paths
		std::vector<cherry::Path> paths;
		bool pathsSet = false;

	protected:

	};
}