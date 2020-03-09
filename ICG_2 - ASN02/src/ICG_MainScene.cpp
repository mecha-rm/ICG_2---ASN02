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

// the maximum amount of post processed lights. 
#define MAX_POST_LIGHTS 25

// Main Scene
#include "ICG_MainScene.h"
#include "cherry/Game.h"
#include "cherry/objects/Primitives.h"
#include "cherry/utils/Utils.h"
#include <imgui\imgui.h>


// if 'true', then the defaut light is enabled
const bool icg::ICG_MainScene::DEFAULT_LIGHT_ENABLED = false;

// constructor
icg::ICG_MainScene::ICG_MainScene(const std::string a_Name)
	: GameplayScene(a_Name)
{
}

// scene open
void icg::ICG_MainScene::OnOpen()
{
	using namespace cherry;
	cherry::GameplayScene::OnOpen();

	Game* const game = Game::GetRunningGame();
	bool usePaths = false;

	if (game == nullptr)
		return;

	// gets the window size
	glm::ivec2 myWindowSize = game->GetWindowSize();

	// CAMERA
	game->myClearColor = glm::vec4(0.74F, 0.251F, 0.9F, 1.0F);

	// Material::Sptr mat1 = Material::;
	// setting up the camera
	game->myCamera = std::make_shared<Camera>();
	Camera::Sptr& myCamera = game->myCamera; // camera reference
	game->myCameraEnabled = true;

	myCamera->clearColor = game->myClearColor; // setting the clear colour
	myCamera->SetPosition(glm::vec3(-40, 22, 150));
	myCamera->LookAt(glm::vec3(0));


	// sets the camera to perspective mode for the m_Scene.
	myCamera->SetPerspectiveMode(glm::radians(45.0f), 1.0f, 0.001f, 10000.0f);

	// sets the orthographic mode values. False is passed so that the camera starts in perspective mode.
	myCamera->SetOrthographicMode(-5.0f, 5.0f, -5.0f, 5.0f, 0.001f, 10000.0f, false);
	myCamera->targetOffset = cherry::Vec3(0, 5, 12);


	// secondary camera, which is used for UI for the game.
	game->myCameraX = std::make_shared<Camera>();
	Camera::Sptr& myCameraX = game->myCameraX;

	myCameraX->clearColor = game->myClearColor;
	myCameraX->SetPosition(0, 0.001F, 1.0F); // try adjusting the position of the perspecitve cam and orthographic cam
	myCameraX->Rotate(glm::vec3(0.0F, 0.0F, glm::radians(180.0f)));
	myCameraX->LookAt(glm::vec3(0));

	// this camera is used for UI elements
	myCameraX->SetPerspectiveMode(glm::radians(60.0f), 1.0f, 0.01f, 1000.0f, false);
	myCameraX->SetOrthographicMode(-myWindowSize.x / 2.0F, myWindowSize.x / 2.0F, -myWindowSize.y / 2.0F, myWindowSize.y / 2.0F, 0.0f, 1000.0f, true);

	game->SetSkyboxVisible(false);

	// load in objects
	LoadObjects();

	/// FRAME BUFFERS
	// frame buffer
	FrameBuffer::Sptr fb = std::make_shared<FrameBuffer>(myWindowSize.x, myWindowSize.y);

	// scene colour 
	RenderBufferDesc sceneColor = RenderBufferDesc();
	sceneColor.ShaderReadable = true;
	sceneColor.Attachment = RenderTargetAttachment::Color0;
	sceneColor.Format = RenderTargetType::Color24; // loads with RGB

	// scene depth
	RenderBufferDesc sceneDepth = RenderBufferDesc();
	sceneDepth.ShaderReadable = true;
	sceneDepth.Attachment = RenderTargetAttachment::Depth;
	sceneDepth.Format = RenderTargetType::Depth24;

	// colour and depth attachments
	fb->AddAttachment(sceneColor);
	fb->AddAttachment(sceneDepth);

	// registry frame buffer
	Registry().ctx_or_set<FrameBuffer::Sptr>(fb);


	// TODO: lighting controls

	// LIGHT
	// light shader
	// post light
	if (DEFAULT_LIGHT_ENABLED)
	{
		postLight = new PostLight(GetName(), POST_VS, BLINN_PHONG_POST);
		postLight->SetVolumeType(1);
		postLight->position = { 1.0F, 1.0F, 10.0F };
		postLight->color = { 0.214F, 0.892F, 0.528F };
		postLight->attenuation = 1.0F / 10000.0F;
		postLight->shininess = 40.0F;

		postLight->shader->SetUniform("a_LightPos", postLight->position);
		postLight->shader->SetUniform("a_LightColor", postLight->color);
		postLight->shader->SetUniform("a_LightAttenuation", postLight->attenuation);
		postLight->shader->SetUniform("a_MatShininess", postLight->shininess);
		postLight->shader->SetUniform("a_UseClearColor", 1); // uses clear color instead of black

		// light buffer 
		ls_fb = std::make_shared<FrameBuffer>(myWindowSize.x, myWindowSize.y);
		ls_fb->AddAttachment(sceneColor);
		ls_fb->AddAttachment(sceneDepth);

		// adding the layer
		lightLayer = new PostLayer(postLight->shader, ls_fb);  
		layers.push_back(lightLayer);
		enabledLights = 1;
	}
	else
	{
		// LoadFromFile("res/icg_2-asn02-light_info-00.txt");
		// LoadFromFile("res/icg_2-asn02-light_info-01.txt");
		// LoadFromFile("res/icg_2-asn02-light_info-02.txt");
		// LoadFromFile("res/icg_2-asn02-light_info-03.txt");
		LoadFromFile("res/icg_2-asn02-light_info-04.txt");
		// LoadFromFile("res/icg_2-asn02-light_info-05.txt");
		UseClearColor(true);


	}

	// PostLayer* layer = new PostLayer(POST_VS, "res/shaders/post/normal-buffer.fs.glsl");
	depthLayer = new PostLayer(POST_VS, "res/shaders/post/depth-buffer.fs.glsl");
	normalLayer = new PostLayer(POST_VS, "res/shaders/post/normal-buffer.fs.glsl");
	
	// effect layer 1
	{
		Shader::Sptr effectShader = std::make_shared<Shader>();
		effectShader->Load(POST_VS, "res/shaders/post/toon-cel-post.fs.glsl");

		// scene colour 
		RenderBufferDesc ecDesc = RenderBufferDesc();
		ecDesc.ShaderReadable = true;
		ecDesc.Attachment = RenderTargetAttachment::Color0;
		ecDesc.Format = RenderTargetType::Color24; // loads with RGB

		// scene depth
		RenderBufferDesc edDesc = RenderBufferDesc();
		edDesc.ShaderReadable = true;
		edDesc.Attachment = RenderTargetAttachment::Depth;
		edDesc.Format = RenderTargetType::Depth24;

		// frame buffer
		FrameBuffer::Sptr efb = std::make_shared<FrameBuffer>(myWindowSize.x, myWindowSize.y);
		efb->AddAttachment(ecDesc);
		efb->AddAttachment(edDesc);

		effectLayer1 = new PostLayer(effectShader, efb);
	}

	// effect layer 2
	{ 
		Shader::Sptr effectShader = std::make_shared<Shader>();
		effectShader->Load(POST_VS, "res/shaders/post/kernel3.fs.glsl");

		// scene colour 
		RenderBufferDesc ecDesc = RenderBufferDesc();
		ecDesc.ShaderReadable = true;
		ecDesc.Attachment = RenderTargetAttachment::Color0;
		ecDesc.Format = RenderTargetType::Color24; // loads with RGB

		// scene depth
		RenderBufferDesc edDesc = RenderBufferDesc();
		edDesc.ShaderReadable = true;
		edDesc.Attachment = RenderTargetAttachment::Depth;
		edDesc.Format = RenderTargetType::Depth24;

		// frame buffer
		FrameBuffer::Sptr efb = std::make_shared<FrameBuffer>(myWindowSize.x, myWindowSize.y);
		efb->AddAttachment(ecDesc);
		efb->AddAttachment(edDesc);

		// uniforms
		effectShader->SetUniform("xKernel", glm::mat3(
			1.0f / 9.0f * 1.0F, 1.0f / 9.0f * 1.0F, 1.0f / 9.0f * 1.0F,
			1.0f / 9.0f * 1.0F, 1.0f / 9.0f * 1.0F, 1.0f / 9.0f * 1.0F,
			1.0f / 9.0f * 1.0F, 1.0f / 9.0f * 1.0F, 1.0f / 9.0f * 1.0F
		));

		effectLayer2 = new PostLayer(effectShader, efb);
		

	}

	useFrameBuffers = true;

	// TODO: make lights move around. 
	if (usePaths)
	{
		// adding the path
		if (DEFAULT_LIGHT_ENABLED && postLight != nullptr)
		{
			// PATH BEHAVIOUR
			cherry::Path path = Path();

			path.AddNode(90.0F, 83.25F, 5.0F);
			path.AddNode(-112.0F, -130.0F, 12.0F);
			path.AddNode(-210.0F, 0.0F, 10.0F);
			path.AddNode(1.0F, 30.0F, 84.0F);
			path.AddNode(-17.0F, 50.0F, 10.0F);

			// closed loop
			path.SetClosedPath(true);
			path.SetIncrementer(0.1F); // incrementer
			path.SetSpeedControl(true); // speed control

			path.SetStartingPoint(postLight->position);
			postLight->path = path;
		}
		else
		{
			cherry::Path path = Path();

			// upper and lower positions
			glm::vec3 posUpper{ 400.0F, 400.0F, 400.0F };
			glm::vec3 posLower{ -400.0F, -400.0F, -400.0F };

			for (PostLight* light : lights)
			{
				path = Path(light->position);
				path.SetIncrementer(0.5F);
				path.SetSpeedControl(true);

				// the light may travel along a path.
				path.SetClosedPath(true);
				path.SetIncrementer(1.0F * (float)(rand() % 100 + 1) / 500.0F);

				float t = 0;
				glm::vec3 node;

				// if the light should be given a path.
				switch (rand() % 5) // 0 - 4 
				{
				case 0:
				case 1:

					for (int j = 0; j < 5; j++)
					{
						t = 1.0F / (float)(rand() % 200 + 1); // 1 - 500

						// generates random positions for the light to travel to.
						node = glm::vec3(glm::mix(posLower.x, posUpper.x, t), glm::mix(posLower.y, posUpper.y, t), 0);
						path.AddNode(Vec3(node));
					}
					break;
				}

				light->path = path;
				paths.push_back(path);
			}
		}
	}

	// // giving the path to all the lights.
	// for (PostLight* light : lights)
	// {
	// 	light->path = path;
	// 	light->path.SetIncrementer(0.01F);
	// 	light->path.SetStartingPoint(light->position);
	// } 


	// post processing test. 
	if (layers.size() > 0 && false)
	{
		PostLayer * layer = new PostLayer(POST_VS, POST_FS);

		// making the shader
		Shader::Sptr sdr = std::make_shared<Shader>();
		sdr->Load(POST_VS, POST_FS);
		// shader->Link();
		
		// making the output of the framebuffer
		cherry::FrameBuffer::Sptr output = std::make_shared<cherry::FrameBuffer>(myWindowSize.x, myWindowSize.y);
		output->AddAttachment(sceneColor);
		output->AddAttachment(sceneDepth);

		// layers[0]->AddLayer(sdr, output);
		layers.push_back(new PostLayer(sdr, output));
	}
}

// scene close
void icg::ICG_MainScene::OnClose()
{
	using namespace cherry;

	// deleting the single post light
	delete postLight;

	// removing the lsit of point lights
	for (int i = 0; i < lights.size(); i++)
		delete lights[i];

	lights.clear();


	// removes the layers from the vector
	util::removeFromVector<PostLayer>(layers, lightLayer);
	delete lightLayer;

	util::removeFromVector<PostLayer>(layers, depthLayer);
	delete depthLayer;

	util::removeFromVector<PostLayer>(layers, normalLayer);
	delete normalLayer;

	util::removeFromVector<PostLayer>(layers, effectLayer1);
	delete effectLayer1;

	util::removeFromVector<PostLayer>(layers, effectLayer2);
	delete effectLayer2;

	// deleting the ohter lights
	for (PostLight* light : lights)
		delete light;

	lights.clear();

	// deletes the object lists and all objects that are in them.
	cherry::GameplayScene::OnClose();

}

// mouse button pressed.
void icg::ICG_MainScene::MouseButtonPressed(GLFWwindow* window, int button)
{
}

// moused button held.
void icg::ICG_MainScene::MouseButtonHeld(GLFWwindow* window, int button)
{
}

// mouse button has been released.
void icg::ICG_MainScene::MouseButtonReleased(GLFWwindow* window, int button)
{
	// checks each button
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		mbLeft = false;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mbMiddle = false;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mbRight = false;
		break;
	}
}

void icg::ICG_MainScene::KeyPressed(GLFWwindow* window, int key)
{
	cherry::Game* game = cherry::Game::GetRunningGame();

	if (game == nullptr) // if game is 'null', then nothing happens
		return;

	// checks key value.
	switch (key)
	{
	case GLFW_KEY_SPACE:
		game->myCamera->SwitchViewMode();
		break;

		// CAMERA CONTROLS
		// TRANSLATIONS
	case GLFW_KEY_W: // y-direction up
		t_Dir[1] = -1;
		break;

	case GLFW_KEY_S: // y-direction down
		t_Dir[1] = 1;
		break;

	case GLFW_KEY_A: // x-direction left
		t_Dir[0] = 1;
		break;

	case GLFW_KEY_D: // x-direction right
		t_Dir[0] = -1;
		break;

	case GLFW_KEY_Q: // z-direction backward
		t_Dir[2] = 1;
		break;

	case GLFW_KEY_E: // z-direction forward
		t_Dir[2] = -1;
		break;

		// ROTATIONS
	case GLFW_KEY_UP: // y-direction +
		r_Dir[0] = -1;
		break;

	case GLFW_KEY_DOWN: // y-direction -
		r_Dir[0] = 1;
		break;

	case GLFW_KEY_LEFT: // x-direction +
		r_Dir[1] = -1;
		break;

	case GLFW_KEY_RIGHT: // x-direction -
		r_Dir[1] = 1;
		break;

	case GLFW_KEY_PAGE_UP: // z-direction -
		r_Dir[2] = -1;
		break;

	case GLFW_KEY_PAGE_DOWN: // z-direction +
		r_Dir[2] = 1;
		break;

		// resets the camera so that it looks at the origin
	case GLFW_KEY_L:
		if (game->myCamera != nullptr)
			game->myCamera->LookAt(game->myCamera->LookingAt());
		break;

	// Assignment Controls
		// increments the light count.
	case GLFW_KEY_EQUAL:
		EnableLight();
		break;
		// decrements the light count. 
	case GLFW_KEY_MINUS:
		DisableLight();
		break;
		// CONTROLS:
		// TODO: add in switching of effects.

	case GLFW_KEY_1: // one light only.
		key1 = true;
		break;

	case GLFW_KEY_2: // view light volumes.
		SetLightVolumesVisible(!volumesVisible);
		break;

	case GLFW_KEY_3: // position/depth buffer
		EnablePositionView();
		break;

	case GLFW_KEY_4: // normal buffer view
		EnableNormalView();
		break;

	case GLFW_KEY_5: // colour/material view
		EnableMaterialView();
		break;

	case GLFW_KEY_6: // light accumulation buffer
		EnableLightingView();
		break;


	case GLFW_KEY_7: // cycles through light volumes
		CycleLightVolumes();
		break;

	case GLFW_KEY_8: // effect 1
		EnableEffect1();

	case GLFW_KEY_9:
		EnableEffect2();

	case GLFW_KEY_0:
		break;
	}
}

// key held
void icg::ICG_MainScene::KeyHeld(GLFWwindow* window, int key)
{
	cherry::Game* game = cherry::Game::GetRunningGame();

	if (game == nullptr) // if game is 'null', then it is returned
		return;

	switch (key)
	{
		// CAMERA CONTROLS
			// TRANSLATIONS
	case GLFW_KEY_W: // y-direction up
		t_Dir[1] = -1;
		break;

	case GLFW_KEY_S: // y-direction down
		t_Dir[1] = 1;
		break;

	case GLFW_KEY_A: // x-direction left
		t_Dir[0] = 1;
		break;

	case GLFW_KEY_D: // x-direction right
		t_Dir[0] = -1;
		break;

	case GLFW_KEY_Q: // z-direction backward
		t_Dir[2] = 1;
		break;

	case GLFW_KEY_E: // z-direction forward
		t_Dir[2] = -1;
		break;

		// ROTATIONS
	case GLFW_KEY_UP: // y-direction +
		r_Dir[0] = -1;
		break;

	case GLFW_KEY_DOWN: // y-direction -
		r_Dir[0] = 1;
		break;

	case GLFW_KEY_LEFT: // x-direction +
		r_Dir[1] = -1;
		break;

	case GLFW_KEY_RIGHT: // x-direction -
		r_Dir[1] = 1;
		break;

	case GLFW_KEY_PAGE_UP: // z-direction -
		r_Dir[2] = -1;
		break;

	case GLFW_KEY_PAGE_DOWN: // z-direction +
		r_Dir[2] = 1;
		break;
	}
}

// key released
void icg::ICG_MainScene::KeyReleased(GLFWwindow* window, int key)
{
	cherry::Game* game = cherry::Game::GetRunningGame();

	if (game == nullptr) // if game is 'null', then it is returned
		return;

	switch (key)
	{
		// CAMERA CONTROLS
		// TRANSLATIONS
		// y-axis movement
	case GLFW_KEY_W:
	case GLFW_KEY_S:
		t_Dir[1] = 0;
		break;

		// x-axis movement
	case GLFW_KEY_A:
	case GLFW_KEY_D:
		t_Dir[0] = 0;
		break;

		// z-axis movement
	case GLFW_KEY_Q:
	case GLFW_KEY_E:
		t_Dir[2] = 0;
		break;

		// ROTATIONS
		// y-axis rotation
	case GLFW_KEY_UP:
	case GLFW_KEY_DOWN:
		r_Dir[0] = 0;
		break;

		// x-axis rotation
	case GLFW_KEY_LEFT:
	case GLFW_KEY_RIGHT:
		r_Dir[1] = 0;
		break;

		// z-axis rotation
	case GLFW_KEY_PAGE_UP:
	case GLFW_KEY_PAGE_DOWN:
		r_Dir[2] = 0;
		break;
	}
}

// imgui draw
void icg::ICG_MainScene::DrawGui(float deltaTime)
{
	using namespace cherry;

	Game* game = Game::GetRunningGame();

	glm::vec4& myClearColor = game->myClearColor; // clear color
	GLFWwindow* myWindow = game->GetWindow(); // window 


	char myWindowTitle[WINDOW_TITLE_CHAR_MAX]; // window title (char array)
	std::string wtStr = game->GetWindowTitle(); // the window title (as a string)

	// fills the rest of the string with the null termination character.
	wtStr.resize(WINDOW_TITLE_CHAR_MAX, '\0');

	// the game's camera
	Camera::Sptr& myCamera = game->myCamera;

	// temporary integer
	int tempInt = 0;

	// moving lights
	int lightIndex = imguiLightIndex;

	glm::vec2 posLimitsX{ -250.0F, 250.0F };
	glm::vec2 posLimitsY{ -250.0F, 250.0F };
	glm::vec2 posLimitsZ{ 0.0F, 10.0F };

	// copying the string's data into the char array
	memcpy(myWindowTitle, wtStr.c_str(), wtStr.length());

	// Open a new ImGui window
	ImGui::Begin("Colour Picker");

	// Draw widgets here
	// ImGui::SliderFloat4("Color", &myClearColor.x, 0, 1); // Original
	ImGui::ColorPicker4("Color", &myClearColor.x); // new version
	// ImGui::SetWindowSize(ImVec2(500.0F, 500.0F)); // window size for ImGUI Colour Picker (perament)
	// ImGui::SetNextWindowCollapsed(false);
	// ImGui::SetNextWindowPos(ImVec2(-225.0F, 1.0F));
	ImGui::SetNextWindowSize(ImVec2(500.0F, 500.0F)); // window size for ImGUI ColorPicker (variable)
	if (ImGui::InputText("Title", myWindowTitle, 31))
	{
		glfwSetWindowTitle(myWindow, myWindowTitle);
	}

	// text for enabled lights
	tempInt = enabledLights;
	if (ImGui::InputInt("Enabled Lights", &tempInt))
	{
		tempInt = glm::clamp(tempInt, 0, MAX_POST_LIGHTS);
		SetEnabledLights(tempInt);
	}

	// multiple lights
	if (!DEFAULT_LIGHT_ENABLED)
	{
		ImGui::BeginTabBar("Light Position");
		ImGui::BeginTabItem(std::string("Light").c_str());

		// checking the light
		if (ImGui::InputInt("Light", &lightIndex))
		{
			// light index bounds
			lightIndex = glm::clamp(lightIndex, 0, (int)lights.size() - 1);
			lightIndex = glm::clamp(lightIndex, 0, MAX_POST_LIGHTS - 1);

			imguiCoord = lights[lightIndex]->position;
		}

		// TODO: figure out why reposition doesn't work.
		// changing light positions
		if (ImGui::InputFloat("X", &imguiCoord.x))
			lights[lightIndex]->position.x = imguiCoord.x;

		if (ImGui::InputFloat("Y", &imguiCoord.y))
			lights[lightIndex]->position.y = imguiCoord.y;

		if (ImGui::InputFloat("Z", &imguiCoord.z))
			lights[lightIndex]->position.z = imguiCoord.z;

		lights[lightIndex]->Update(0);

		// randomizes position.
		// TODO: fix
		if (ImGui::Button("Randomize"))
		{
			// interpolation
			glm::vec3 temp = glm::vec3(
				glm::mix(posLimitsX.x, posLimitsX.y, (float)(rand() % 1001) / 1000.0F),
				glm::mix(posLimitsY.x, posLimitsY.y, (float)(rand() % 1001) / 1000.0F),
				glm::mix(posLimitsZ.x, posLimitsZ.y, (float)(rand() % 1001) / 1000.0F)
			);

			lights[lightIndex]->position = temp;
			lights[lightIndex]->Update(0);
			imguiCoord = temp;
		}

		imguiLightIndex = lightIndex;
		ImGui::EndTabItem();

		ImGui::EndTabBar();
	}

	// original
	// ImGui::BeginTabBar("Lights");
	// {
	// 	int light = 0;
	// 	float coord = 0;
	// 	
	// 	glm::vec2 posLimitsX{ -250.0F, 250.0F };
	// 	glm::vec2 posLimitsY{ -250.0F, 250.0F };
	// 	glm::vec2 posLimitsZ{  0.0F, 10.0F };
	// 
	// 	for (int i = 0; i < lights.size() && i < MAX_POST_LIGHTS; i++)
	// 	{
	// 		ImGui::BeginTabItem(std::string("Light " + std::to_string(i)).c_str());
	// 
	// 		// changing light positions
	// 		if (ImGui::InputFloat("X", &coord))
	// 			lights[i]->position.x = coord;
	// 
	// 		if (ImGui::InputFloat("Y", &coord))
	// 			lights[i]->position.x = coord;
	// 
	// 		if (ImGui::InputFloat("Z", &coord))
	// 			lights[i]->position.x = coord;
	// 		
	// 		// randomizes position.
	// 		if (ImGui::Button("Randomize"))
	// 		{
	// 			// t is equal to a random value between 0 and 1000 dividied by 1000. It is within the range [0, 1].
	// 			float t = (float)(rand() % 1001) / 1000.0F;
	// 			
	// 			lights[0]->position = glm::vec3(
	// 				glm::mix(posLimitsX.x, posLimitsX.y, t),
	// 				glm::mix(posLimitsY.x, posLimitsY.y, t),
	// 				glm::mix(posLimitsZ.x, posLimitsZ.y, t)
	// 			);
	// 		}
	// 	}
	// 
	// }
	// ImGui::EndTabBar();


	if (ImGui::Button("Apply")) // adding another button, which allows for the application of the window title.
	{
		glfwSetWindowTitle(myWindow, myWindowTitle);
	}
	if (ImGui::Button("Wireframe/Fill Toggle"))
	{
		for (cherry::Object* obj : objectList->objects)
			obj->SetWireframeMode();
	}

	// changing the camera mode
	std::string camMode = myCamera->InPerspectiveMode() ? "Perspective" : "Orthographic";
	ImGui::InputText((std::string("CAMERA MODE (\'SPACE\')") + camMode).c_str(), myWindowTitle, WINDOW_TITLE_CHAR_MAX);

	ImGui::End();
}

// activates a light
void icg::ICG_MainScene::EnableLight() { LightActivation(true); }

// deactivates a light
void icg::ICG_MainScene::DisableLight() { LightActivation(false); }

// sets the amount of enabled lights.
void icg::ICG_MainScene::SetEnabledLights(int enabled)
{
	if (enabled < 0 || enabled > MAX_POST_LIGHTS)
		return;
	
	// only total amount of lights can be used.
	enabled = glm::clamp(enabled, 0, (int)lights.size());

	// if lights are being enabled
	if (enabled > enabledLights)
	{
		// enabling the requested amount of lights
		while(enabledLights < enabled)
			LightActivation(true);

	}
	else if (enabled < enabledLights) // lights are being disabled
	{
		// disabling lights the requested amount of lights
		while (enabledLights > enabled)
			LightActivation(false);
	}
}

// returns 'true' if the volumes are visible.
bool icg::ICG_MainScene::GetLightVolumesVisible() const { return volumesVisible; }

// sets if the light volumes should be visible or not.
void icg::ICG_MainScene::SetLightVolumesVisible(bool visible)
{
	// turning on the default light.
	if (DEFAULT_LIGHT_ENABLED && visible)
	{
		postLight->SetVisible(visible);
	}
	else
	{
		// enabling or disabling all lights in the light lsit.
		for (PostLight* pl : lights)
			pl->SetVisible(visible);
	}

	volumesVisible = visible;

}

// cycles through light volume types.
void icg::ICG_MainScene::CycleLightVolumes()
{
	const int COUNT = PostLight::GetVolumeTypeCount(); // amount of light types

	if (DEFAULT_LIGHT_ENABLED && postLight != nullptr)
	{
		// moving to a new type
		int index = postLight->GetVolumeType();
		index++;

		// looping back around.
		if (index >= COUNT)
			index = 0;

		postLight->SetVolumeType(index);
	}
	else
	{
		// going through all the lights
		for (int i = 0; i < lights.size(); i++)
		{
			// moving to a new type
			int index = lights[i]->GetVolumeType();
			index++;

			// looping back around.
			if (index >= COUNT)
				index = 0;

			lights[i]->SetVolumeType(index);
		}
	}
}

// enables position and depth buffer view
void icg::ICG_MainScene::EnablePositionView()
{
	using namespace cherry;

	const Game* game = Game::GetRunningGame();

	// enables depth layer only.
	layers.clear();
	layers.push_back(depthLayer);

	// updating the size of the buffer
	depthLayer->OnWindowResize(game->GetWindowWidth(), game->GetWindowHeight());
}

// enables the nomral view
void icg::ICG_MainScene::EnableNormalView()
{
	using namespace cherry;

	const Game* game = Game::GetRunningGame();

	// enabling normal layer only
	layers.clear();
	layers.push_back(normalLayer);

	// updating the size of the buffer
	normalLayer->OnWindowResize(game->GetWindowWidth(), game->GetWindowHeight());
}

// enables the view of the objects.
void icg::ICG_MainScene::EnableMaterialView()
{
	// no post processing layers being used.
	layers.clear();
}

// lighting view
void icg::ICG_MainScene::EnableLightingView()
{
	using namespace cherry;

	const Game* game = Game::GetRunningGame();

	layers.clear();
	layers.push_back(lightLayer);

	// updating the size of the buffer
	lightLayer->OnWindowResize(game->GetWindowWidth(), game->GetWindowHeight());
}

// enables the first effect
void icg::ICG_MainScene::EnableEffect1()
{
	using namespace cherry;

	if (effectLayer1 == nullptr)
		return;

	const Game* game = Game::GetRunningGame();

	layers.clear();
	layers.push_back(lightLayer);
	layers.push_back(effectLayer1);

	// updating the size of the buffer
	lightLayer->OnWindowResize(game->GetWindowWidth(), game->GetWindowHeight());
	effectLayer1->OnWindowResize(game->GetWindowWidth(), game->GetWindowHeight());
}

// enables effect 2
void icg::ICG_MainScene::EnableEffect2()
{
	using namespace cherry;

	if (effectLayer2 == nullptr)
		return;

	const Game* game = Game::GetRunningGame();

	layers.clear();
	layers.push_back(lightLayer);
	layers.push_back(effectLayer2);

	// updating the size of the buffer
	lightLayer->OnWindowResize(game->GetWindowWidth(), game->GetWindowHeight());
	effectLayer2->OnWindowResize(game->GetWindowWidth(), game->GetWindowHeight());
}

// loads the lights from a file
void icg::ICG_MainScene::LoadFromFile(std::string filePath)
{
	using namespace cherry;
	
	// gets the running game
	const Game* game = Game::GetRunningGame();

	// opening the file
	std::ifstream file(filePath, std::ios::in);

	if (!file)
	{
		std::runtime_error("File cannot be opened.");
		file.close();
		return;
	}

	// no game is running.
	if (game == nullptr)
		return;

	// Read Variables
	// a line from the file.
	std::string line;
	std::vector<std::string> comps; // components

	// post-processing shader
	cherry::Shader::Sptr ls_shader = std::make_shared<Shader>();
	cherry::Material::Sptr ls_material;

	// light shader
	ls_shader->Load(POST_VS, BLINN_PHONG_POST_MULTI);
	ls_material = std::make_shared<Material>(ls_shader);

	// scene colour
	RenderBufferDesc sceneColor = RenderBufferDesc();
	sceneColor.ShaderReadable = true;
	sceneColor.Attachment = RenderTargetAttachment::Color0;
	sceneColor.Format = RenderTargetType::Color24; // loads with RGB

	// scene depth
	RenderBufferDesc sceneDepth = RenderBufferDesc();
	sceneDepth.ShaderReadable = true;
	sceneDepth.Attachment = RenderTargetAttachment::Depth;
	sceneDepth.Format = RenderTargetType::Depth24;

	// light buffer 
	ls_fb = std::make_shared<FrameBuffer>((float)game->GetWindowWidth(), (float)game->GetWindowHeight());
	ls_fb->AddAttachment(sceneColor);
	ls_fb->AddAttachment(sceneDepth);

	// adding the post processing layer. 
	lightLayer = new PostLayer(ls_shader, ls_fb);
	layers.push_back(lightLayer);
	
	// int index = 0;

	while (std::getline(file, line))
	{
		// splits a string based on the spaces in it.
		// although it's separated by tabs, spacing will be detected as the same thing.
		comps = util::splitString<std::string>(line);

		if (comps[0] == "Enabled") // amount of enabled lights
		{
			// TODO: is this needed?
			// lights.resize(util::convertString<int>(comps[1]));

			// setting the amount of enabled lights (max is 25)
			ls_shader->SetUniform("a_EnabledLights", util::convertString<int>(comps[1]));
		}
		else if (util::isInt(comps[0])) // number value of some sort.
		{
			// Order
			// Number/Type/PositionX/PositionY/PositionZ/ColorRed/ColorGreen/ColorBlue/Attenuation/Shininess 
	
			// passing in the scene name, the material, and the index.
			PostLight* light = new PostLight(GetName(), ls_material, util::convertString<int>((comps[0]))); // light
			 
			// index
			// light->index = util::convertString<int>((comps[0])); 

			// Type (1 = sphere, 2 = cube, 3 = cone)
			light->SetVolumeType(util::convertString<int>((comps[1])) - 1);

			// Position (x, y, z)
			light->position.x = util::convertString<float>((comps[2]));
			light->position.y = util::convertString<float>((comps[3]));
			light->position.z = util::convertString<float>((comps[4]));


			// Color (r, g, b)
			light->color.r = util::convertString<float>((comps[5]));
			light->color.g = util::convertString<float>((comps[6]));
			light->color.b = util::convertString<float>((comps[7]));

			// Attenuation
			light->attenuation = util::convertString<float>((comps[8]));

			// Shininess
			light->shininess = util::convertString<float>((comps[9]));
			// index++;
			// }
			light->path = Path(light->position);
			light->path.SetClosedPath(true);

			// changing values in shader
			light->Update(0);

			// pushing back the light
			lights.push_back(light);
		}
	}

	file.close();

	// enabled lights
	enabledLights = lights.size();
}

// loads objects into the scene.
void icg::ICG_MainScene::LoadObjects()
{
	using namespace cherry;

	float xOffset = 60.0F;
	float yOffset = 60.0F;

	// rotation factors
	float rot0 = 12.5F, rot1 = 24.5F, rot2 = -14.4F;

	// lights
	// this light doesn't actually do anything.
	Light* light = new Light(GetName(), Vec3(1.0F, 0.0F, -4.0F), Vec3(0.5F, 0.2F, 0.1F), Vec3(0.2F, 0.4F, 0.9F), 0.2, 0.2, 1.0F, 1.0F);
	lightList->AddLight(light);


	// objectMat = lightList->GenerateMaterial(STATIC_VS, STATIC_FS);
	Material::Sptr objectMat = lightList->GenerateMaterial("res/shaders/shader.vs.glsl", "res/shaders/shader.fs.glsl");
	// volumeMat = lightList->GenerateMaterial("res/shaders/shader.vs.glsl", "res/shaders/shader.fs.glsl");

	// TODO: aesthetically pleasing scene
	// TODO: moving or orientating scene.

	// primitive
	Primitive* prim = nullptr;

	{
		prim = new PrimitiveUVSphere(5.0F, 12, 12, Vec4(0.3F, 1.0F, 0.7F, 1.0F));
		prim->SetPosition(xOffset * -1, yOffset * -1, 2.0F);
		prim->CreateEntity(GetName(), objectMat);

		objectList->AddObject(prim);
		objRots.push_back(ObjRotate{ prim, glm::vec3(rot1, -rot2, rot0) });
	}

	{
		prim = new PrimitiveCube(7.0F, 2.0F, 4.5F, Vec4(0.9F, 0.3F, 0.54F, 1.0F));
		prim->SetPosition(xOffset * 0, yOffset * -1, 1.0F);
		prim->CreateEntity(GetName(), objectMat);

		objectList->AddObject(prim);
		objRots.push_back(ObjRotate{ prim, glm::vec3(-rot2, rot0, rot1) });
	}

	{
		prim = new PrimitiveCylinder(5.0F, 15, 5, Vec4(0.111F, 0.421F, 0.231F, 1.0F));
		prim->SetPosition(xOffset * 1, yOffset * -1, 9.0F);
		prim->CreateEntity(GetName(), objectMat);

		objectList->AddObject(prim);
		objRots.push_back(ObjRotate{ prim, glm::vec3(-rot1, rot0, rot2) });
	}

	{
		prim = new PrimitiveDiamond(5.0F, 10.0F, 8, 0.5F, Vec4(0.2121F, 0.121F, 0.431F, 1.0F));
		prim->SetPosition(xOffset * -1, yOffset * 0, 9.0F);
		prim->CreateEntity(GetName(), objectMat);
		
		objectList->AddObject(prim);
		objRots.push_back(ObjRotate{ prim, glm::vec3(rot0, rot2 * rot1, -rot1) });
	}

	{
		prim = new PrimitiveCapsule(10.0F, 19.0F, 12, 10, Vec4(0.9221F, 0.521F, 0.1431F, 1.0F));
		prim->SetPosition(xOffset * 0, yOffset * 0, 40.0F);
		prim->CreateEntity(GetName(), objectMat);
		prim->Rotate(Vec3(45.0F, 0.0F, 0.0F), true);

		objectList->AddObject(prim);
		objRots.push_back(ObjRotate{ prim, glm::vec3(rot2, rot0, -rot1) });
	}

	// platform
	prim = new Plane(500.0F, 500.0F, true, Vec4(0.4F, 0.4F, 0.4F, 1.0F));
	prim->SetPosition(0, 0, -5);
	prim->CreateEntity(GetName(), objectMat);
	objectList->AddObject(prim);

	// wall
	prim = new Plane(50.0F, 50.0F, true, Vec4(1.0F, 0.2F, 0.4F, 1.0F));
	prim->SetPosition(0, -100, 25.0f);
	prim->CreateEntity(GetName(), objectMat);
	prim->RotateY(90.0F, true);
	objectList->AddObject(prim);


	// prim = new PrimitiveUVSphere(5.0F, 12, 12, Vec4(0.3F, 1.0F, 0.7F, 1.0F));
	// prim->SetPosition(10.0F, -2.0F, 1.0F);
	// prim->CreateEntity(GetName(), prim->GetMaterial());
	// objectList->AddObject(prim);
}

// activates (or deactivates) a light
void icg::ICG_MainScene::LightActivation(bool activate)
{
	// using the default light
	if (DEFAULT_LIGHT_ENABLED && postLight != nullptr)
	{
		enabledLights = 1;
		return;
	}

	// no lights in the list.
	if (lights.size() == 0)
		return;

	// if all the lights are enabled and another light is trying to be enabled, nothing happens.
	// also, if another light is being disabled, but all lights are already disabled, then nothing happens.
	if ((activate == true && enabledLights + 1 > lights.size()) || (activate == false && enabledLights - 1 < 0))
		return;

	// activate
	switch (activate)
	{
	case true: // showing a light
		lights[enabledLights]->SetVisible(true);
		enabledLights++;
		break;

	case false: // hiding a light
	default:
		enabledLights--;
		lights[enabledLights]->SetVisible(false);
		break;
	}

	lights[0]->shader->SetUniform("a_EnabledLights", enabledLights);
}

// sets whether to use the clear colour or not.
void icg::ICG_MainScene::UseClearColor(bool useClear)
{
	if(DEFAULT_LIGHT_ENABLED && postLight != nullptr)
		postLight->shader->SetUniform("a_UseClearColor", (int)useClear);

	if (lights.size() > 0)
		lights[0]->shader->SetUniform("a_UseClearColor", (int)useClear);
}

// update
void icg::ICG_MainScene::Update(float deltaTime)
{
	using namespace cherry;
	cherry::GameplayScene::Update(deltaTime);

	cherry::Game* game = Game::GetRunningGame();

	game->myCamera->SetPosition(game->myCamera->GetPosition()
		+ glm::vec3(t_Dir[0] * t_Inc * deltaTime, t_Dir[1] * t_Inc * deltaTime, t_Dir[2] * t_Inc * deltaTime));

	// rotating the camera
	game->myCamera->Rotate(
		glm::vec3(glm::radians(r_Dir[0] * r_Inc * deltaTime),
			glm::radians(r_Dir[1] * r_Inc * deltaTime),
			glm::radians(r_Dir[2] * r_Inc * deltaTime)
		)
	);

	// settng the paths because they weren't set before for some reason.
	if (!DEFAULT_LIGHT_ENABLED && pathsSet == false)
	{
		for (int i = 0; i < paths.size() && i < lights.size(); i++)
		{
			paths[i].SetStartingPoint(lights[0]->position);
			paths[i].SetClosedPath(true);
			lights[i]->path = paths[i];
		}
		pathsSet = true;
	}

	// update post light
	if (DEFAULT_LIGHT_ENABLED && postLight != nullptr)
		postLight->Update(deltaTime);

	
	// making only the first body visible
	if (key1 && lights.size() != 0)
	{
		SetEnabledLights(1);
	}

	// rotates the objects
	for (ObjRotate oRot : objRots)
	{
		if (oRot.object != nullptr)
			oRot.object->Rotate(oRot.rInc * deltaTime, true);
	}

	// updating the lights
	for (PostLight* light : lights)
		light->Update(deltaTime);

	key1 = false;
}
