/*
 * Name:
 * Date:
 * Description:
 * References:
	* http://www.cplusplus.com/reference/vector/vector/resize/
*/


// Main Scene
#include "ICG_MainScene.h"
#include "cherry/Game.h"
#include "cherry/objects/Primitives.h"
#include "cherry/utils/Utils.h"


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

	if (game == nullptr)
		return;

	// gets the window size
	glm::ivec2 myWindowSize = game->GetWindowSize();
	Material::Sptr objectMat; // material for objects
	Material::Sptr volumeMat; // material for light volumes

	// CAMERA
	game->myClearColor = glm::vec4(0.74F, 0.251F, 0.9F, 1.0F); 

	// Material::Sptr mat1 = Material::;
	// setting up the camera
	game->myCamera = std::make_shared<Camera>();
	Camera::Sptr& myCamera = game->myCamera; // camera reference
	game->myCameraEnabled = true;

	myCamera->clearColor = game->myClearColor; // setting the clear colour
	myCamera->SetPosition(glm::vec3(-20, 12, 50));
	myCamera->LookAt(glm::vec3(0));


	// sets the camera to perspective mode for the m_Scene.
	myCamera->SetPerspectiveMode(glm::radians(45.0f), 1.0f, 0.01f, 1000.0f);

	// sets the orthographic mode values. False is passed so that the camera starts in perspective mode.
	myCamera->SetOrthographicMode(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 100.0f, false);
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

	// lights
	Light* light = new Light(GetName(), Vec3(1.0F, 0.0F, -4.0F), Vec3(0.5F, 0.2F, 0.1F), Vec3(0.2F, 0.4F, 0.9F), 0.2, 0.2, 1.0F, 1.0F);
	lightList->AddLight(light);

	light = new Light(GetName(), Vec3(50.0F, -2.0F, -3.0F), Vec3(0.994F, 0.3F, 0.142F), Vec3(0.43F, 0.7F, 0.859F), 3, 0.01, 150.0F, 2.0F);
	lightList->AddLight(light);


	// making the material without lights.
	// objectMat = lightList->GenerateMaterial(STATIC_VS, STATIC_FS);
	objectMat = lightList->GenerateMaterial("res/shaders/shader.vs.glsl", "res/shaders/shader.fs.glsl");
	volumeMat = lightList->GenerateMaterial("res/shaders/shader.vs.glsl", "res/shaders/shader.fs.glsl");

	// primitive
	Primitive* prim = nullptr;

	prim = new PrimitiveUVSphere(5.0F, 12, 12, Vec4(0.3F, 1.0F, 0.7F, 1.0F));
	prim->SetPosition(0.0F, 0.0F, 2.0F);
	prim->CreateEntity(GetName(), objectMat);
	objectList->AddObject(prim);


	prim = new PrimitiveCube(7.0F, 2.0F, 4.5F, Vec4(0.9F, 0.3F, 0.54F, 1.0F));
	prim->SetPosition(5.0F, 10.0F, 1.0F);
	prim->CreateEntity(GetName(), objectMat);
	objectList->AddObject(prim);

	prim = new PrimitiveCylinder(2.0F, 15, 5, Vec4(0.111F, 0.421F, 0.231F, 1.0F));
	prim->SetPosition(6.0F, 3.1842F, 9.0F);
	prim->CreateEntity(GetName(), objectMat);
	objectList->AddObject(prim);

	prim = new Plane(300.0F, 300.0F, false, Vec4(0.4F, 0.4F, 0.4F, 1.0F));
	prim->SetPosition(0, 0, 0);
	prim->CreateEntity(GetName(), objectMat);
	objectList->AddObject(prim);

	// prim = new PrimitiveUVSphere(5.0F, 12, 12, Vec4(0.3F, 1.0F, 0.7F, 1.0F));
	// prim->SetPosition(10.0F, -2.0F, 1.0F);
	// prim->CreateEntity(GetName(), prim->GetMaterial());
	// objectList->AddObject(prim);

	// TODO: aesthetically pleasing scene
	// TODO: moving or orientating scene.

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

	// fb->AddAttachment()
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
	}

	// light buffer 
	ls_fb = std::make_shared<FrameBuffer>(myWindowSize.x, myWindowSize.y);
	ls_fb->AddAttachment(sceneColor);
	ls_fb->AddAttachment(sceneDepth);

	// adding the post processing layer. 
	layers.push_back(new PostLayer(postLight->shader, ls_fb));
	// post-> 
	 
	// adds a post-processing 
	// layers.push_back(new PostLayer(POST_VS, "res/shaders/post/invert.fs.glsl"));
	// layers.push_back(new PostLayer(POST_VS, "res/shaders/post/greyscale.fs.glsl"));

	useFrameBuffers = true;

	// TODO: make lights move around. 

	// PATH BEHAVIOUR
	cherry::Path path = Path(postLight->position);

	path.AddNode(90.0F, 83.25F, 5.0F);
	path.AddNode(-112.0F, -130.0F, 12.0F);
	path.AddNode(-210.0F, 0.0F, 10.0F); 
	path.AddNode(1.0F, 30.0F, 84.0F);
	path.AddNode(-17.0F, 50.0F, 10.0F);

	// closed loop
	path.SetClosedPath(true);
	path.SetIncrementer(0.1F); // incrementer
	path.SetSpeedControl(true); // speed control

	
	if(DEFAULT_LIGHT_ENABLED && postLight != nullptr)
		postLight->path = path;
}

// scene close
void icg::ICG_MainScene::OnClose()
{
	// deleting the single post light
	delete postLight;

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

		// CONTROLS:
		// TODO: add in switching of effects.
	case GLFW_KEY_1:
	case GLFW_KEY_2:
	case GLFW_KEY_3:
	case GLFW_KEY_4:
	case GLFW_KEY_5:
	case GLFW_KEY_6:
	case GLFW_KEY_7:
	case GLFW_KEY_8:
	case GLFW_KEY_9:
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

		// deletes an object
	case GLFW_KEY_0:
		game->DeleteObjectFromScene(objectList->objects.at(0));
		break;
	}
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

	// light shader
	ls_shader->Load(POST_VS, BLINN_PHONG_POST_MULTI);

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
	ls_fb = std::make_shared<FrameBuffer>(game->GetWindowWidth(), game->GetWindowHeight());
	ls_fb->AddAttachment(sceneColor);
	ls_fb->AddAttachment(sceneDepth);

	// adding the post processing layer. 
	layers.push_back(new PostLayer(ls_shader, ls_fb));
	
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
		}
		else if (util::isInt(comps[0])) // number value of some sort.
		{
			// Order
			// Number/PositionX/PositionY/PositionZ/ColorRed/ColorGreen/ColorBlue/Attenuation/Shininess
			// if (index < comps.size())
			// {
			PostLight* light = new PostLight(GetName(), ls_shader); // light

			// index
			light->index = util::convertString<int>((comps[0]));

			// Position (x, y, z)
			light->position.x = util::convertString<float>((comps[1]));
			light->position.y = util::convertString<float>((comps[2]));
			light->position.z = util::convertString<float>((comps[3]));

			// Color (r, g, b)
			light->color.r = util::convertString<float>((comps[4]));
			light->color.g = util::convertString<float>((comps[5]));
			light->color.b = util::convertString<float>((comps[6]));

			// Attenuation
			light->attenuation = util::convertString<float>((comps[7]));

			// Shininess
			light->shininess = util::convertString<float>((comps[8]));
			// index++;
			// }

			// changing values in shader
			light->Update(0);

			// pushing back the light
			lights.push_back(light);
		}
	}

	file.close();
}

// sets whether to use the clear colour or not.
void icg::ICG_MainScene::UseClearColor(bool useClear)
{
	if(DEFAULT_LIGHT_ENABLED && postLight != nullptr)
		postLight->shader->SetUniform("a_UseClearColor", (int)useClear);

	for(PostLight* pl : lights)
		pl->shader->SetUniform("a_UseClearColor", (int)useClear);
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
	
	// update post light
	if (DEFAULT_LIGHT_ENABLED && postLight != nullptr)
		postLight->Update(deltaTime);

	// updates all lights
	for (PostLight* light : lights)
		light->Update(deltaTime);
}
