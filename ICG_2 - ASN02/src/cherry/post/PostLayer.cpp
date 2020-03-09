#include "PostLayer.h"
#include "..\utils\Utils.h"
#include "..\Game.h"

// post layer; makes the quad that will be overlayed.
cherry::PostLayer::PostLayer()
{
	// this was moved from the AddLayer function since the quad only needs to be made once.
	// making the vertices for the quad
	Vertex* verts = new Vertex[4]
	{
		//  {nx, ny, nz}, {r, g, b, a}, {nx, ny, nz}, {u, v}
		{{ -1.0F, -1.0F, 0.0f }, { 1.0F, 1.0F, 1.0F, 1.0F }, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F}}, // bottom left
		{{  1.0F, -1.0F, 0.0f }, { 1.0F, 1.0F, 1.0F, 1.0F }, {0.0F, 0.0F, 1.0F}, {1.0F, 0.0F}}, // bottom right
		{{ -1.0F,  1.0F, 0.0f }, { 1.0F, 1.0F, 1.0F, 1.0F }, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F}}, // top left
		{{  1.0F,  1.0F, 0.0f }, { 1.0F, 1.0F, 1.0F, 1.0F }, {0.0F, 0.0F, 1.0F}, {1.0F, 1.0F}}, // top right
	};

	// making the indices for the quad
	uint32_t* indices = new uint32_t[6]{
		0, 1, 2,
		2, 1, 3
	};

	// full screen quadrilateral
	myFullscreenQuad = std::make_shared<cherry::Mesh>(verts, 4, indices, 6);

	// deleting the vertices and indices
	delete[] verts;
	delete[] indices;
}

// post processing layer
cherry::PostLayer::PostLayer(const std::string vs, const std::string fs)
	: PostLayer()
{ 
	AddLayer(vs, fs);
}

// creates a post processing layer with a shader and frame buffer
cherry::PostLayer::PostLayer(Shader::Sptr& shader, FrameBuffer::Sptr& output)
	: PostLayer()
{
	AddLayer(shader, output);
}

// post-processing layer
void cherry::PostLayer::AddLayer(const std::string vs, const std::string fs)
{
	const Game* const currGame = Game::GetRunningGame();
	  
	// making the render buffer
	RenderBufferDesc mainColor = RenderBufferDesc();
	mainColor.ShaderReadable = true;
	mainColor.Attachment = RenderTargetAttachment::Color0;
	mainColor.Format = RenderTargetType::Color24;

	// making the shader
	Shader::Sptr shader = std::make_shared<Shader>();
	shader->Load(vs.c_str(), fs.c_str());
	// shader->Link();

	// making the output of the framebuffer
	cherry::FrameBuffer::Sptr output = std::make_shared<cherry::FrameBuffer>(currGame->GetWindowWidth(), currGame->GetWindowHeight());
	output->AddAttachment(mainColor);
	output->Validate();

	// Add the pass to the post processing stack
	myPasses.push_back({ shader, output });
}

// adds a layer
void cherry::PostLayer::AddLayer(Shader::Sptr& shader, FrameBuffer::Sptr& output)
{
	if (shader == nullptr || output == nullptr)
		std::runtime_error("Null layer is prohibited.");

	// Add the pass to the post processing stack
	myPasses.push_back({ shader, output });
}

// resizes the layers
void cherry::PostLayer::OnWindowResize(uint32_t width, uint32_t height)
{
	for (auto& pass : myPasses)
		pass.Output->Resize(width, height);
}

// renders the post layer
void cherry::PostLayer::PostRender(const cherry::Camera::Sptr& camera)
{
	// gets the game being run for its screen size.
	const Game* const game = Game::GetRunningGame(); 
	// auto& ecs = CurrentRegistry();
	int depthCheck = GL_DEPTH_FUNC;
	// std::cout << "Depth Check 1: " << std::boolalpha << depthCheck << std::endl;

	// We grab the application singleton to get the size of the screen
	// ecs.ctx_or_set<AppFrameState>();
	// CurrentRegistry().ctx_or_set<FrameBuffer::Sptr>();

	// the main buffer
	FrameBuffer::Sptr mainBuffer;

	// the main buffer
	mainBuffer = (initialBuffer == nullptr) ?
		CurrentRegistry().ctx<FrameBuffer::Sptr>() :
		initialBuffer;

	// The last output will start as the output from the rendering
	FrameBuffer::Sptr& lastPass = mainBuffer;

	// getting the near and far planes.
	float nearPlane = camera->IsPerspectiveCamera() ? camera->GetNearPerspective() : camera->GetNearOrthographic();
	float farPlane = camera->IsPerspectiveCamera() ? camera->GetFarPerspective() : camera->GetFarOrthographic();

	glDisable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	// depthCheck = GL_DEPTH_FUNC;
	// std::cout << "Depth Check 2: " << std::boolalpha << depthCheck << std::endl;
	 
	// We'll iterate over all of our render passes
	for (const PostPass& pass : myPasses) {
	// for (int i = 0; i < myPasses.size(); i++) {
	// 	PostPass& pass = myPasses[i];

		// We'll bind our post-processing output as the current render target and clear it
		pass.Output->Bind(RenderTargetBinding::Draw);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // the original version only clears the colour buffer
		// Set the viewport to be the entire size of the passes output
		glViewport(0, 0, pass.Output->GetWidth(), pass.Output->GetHeight());

		// Use the post processing shader to draw the fullscreen quad
		pass.Shader->Bind(); // binds the shader so that it can be used.
		
		lastPass->GetAttachment(RenderTargetAttachment::Color0)->Bind(0);
		pass.Shader->SetUniform("xImage", 0);


		// camera components for the shaders.
		pass.Shader->SetUniform("a_View", camera->GetView());
		pass.Shader->SetUniform("a_Projection", camera->GetProjection());
		pass.Shader->SetUniform("a_ProjectionInv", glm::inverse(camera->GetProjection()));
		pass.Shader->SetUniform("a_ViewProjection", camera->GetViewProjection());
		pass.Shader->SetUniform("a_ViewProjectionInv", glm::inverse(camera->GetViewProjection()));
		
		// this is supposed to be from the last pass, but these components don't change between passes.
		pass.Shader->SetUniform("a_PrevView", camera->GetView());
		pass.Shader->SetUniform("a_PrevProjection", camera->GetProjection());
		pass.Shader->SetUniform("a_PrevProjectionInv", glm::inverse(camera->GetProjection()));
		pass.Shader->SetUniform("a_PrevViewProjection", camera->GetViewProjection());
		pass.Shader->SetUniform("a_PrevViewProjectionInv", glm::inverse(camera->GetViewProjection()));

		// near and far plane
		pass.Shader->SetUniform("a_NearPlane", nearPlane);
		pass.Shader->SetUniform("a_FarPlane", farPlane);

		// binding the textures in order
		// the frame buffers do not have any input component, so this loop cannot be used.
		// for (size_t ix = 0; ix < pass.Inputs.size(); ix++) {
		// 	const auto& input = pass->Inputs[ix];
		// 	if (input.Pass == nullptr) {
		// 		if (input.UsePrevFrame && state.Last.Output != nullptr) {
		// 			state.Last.Output->Bind(ix + 1, input.Attachment);
		// 		}
		// 		else {
		// 			mainBuffer->Bind(ix + 1, input.Attachment);
		// 		}
		// 	}
		// 	else {
		// 		input.Pass->Output->Bind(ix + 1, input.Attachment);
		// 	}
		// }  

		// post processed lights 
		lastPass->Bind(1, RenderTargetAttachment::Depth); 
		lastPass->Bind(2, RenderTargetAttachment::Color0);

		pass.Shader->SetUniform("xScreenRes", glm::vec2(pass.Output->GetWidth(), pass.Output->GetHeight()));
	
		// if(i == myPasses.size() - 1)
		myFullscreenQuad->Draw(); 

		// Unbind the output pass so that we can read from it
		pass.Output->UnBind();
		// Update the last pass output to be this passes output
		lastPass = pass.Output;
		


	}
	// std::cout << std::endl;
	
	// Bind the last buffer we wrote to as our source for read operations
	lastPass->Bind(RenderTargetBinding::Read);

	// Copies the image from lastPass into the default back buffer
	FrameBuffer::Blit({ 0, 0, lastPass->GetWidth(), lastPass->GetHeight() },
		{ 0, 0, game->GetWindowWidth(), game->GetWindowHeight() }, BufferFlags::All, MagFilter::Nearest);


	// Unbind the last buffer from read operations, so we can write to it again later
	lastPass->UnBind();
	initialBuffer = nullptr;

	glEnable(GL_DEPTH_TEST);
	// glDepthMask(GL_TRUE);

	// depthCheck = GL_DEPTH_FUNC;
	// std::cout << "Depth Check 3: " << std::boolalpha << depthCheck << "\n" << std::endl;

	// clears initial buffer for next post layer pass
	initialBuffer = nullptr;

	// CurrentRegistry().ctx_or_set<FrameBuffer::Sptr>(lastPass);
}

// returns the shader from the last pass
const cherry::Shader::Sptr& cherry::PostLayer::GetLastPassShader()
{
	if (!myPasses.empty()) // there are layers
	{
		return myPasses.at(myPasses.size() - 1).Shader;
	}
	else // no layers
	{
		return nullptr;
	}
}

// returns the frame buffer from the last pass
const cherry::FrameBuffer::Sptr& cherry::PostLayer::GetLastPassBuffer()
{
	if (!myPasses.empty()) // there are layers
	{
		return myPasses.at(myPasses.size() - 1).Output;
	}
	else // there are no layers
	{
		return nullptr;
	}
}