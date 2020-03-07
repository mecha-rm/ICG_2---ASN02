#include "PostLight.h"
#include "cherry/Game.h"

// default constructor.
icg::PostLight::PostLight(std::string sceneName)
	: PostLight(sceneName, std::make_shared<cherry::Shader>(), -1)
{
}

// creates a post light with a vertex and fragment shader
icg::PostLight::PostLight(std::string sceneName, std::string vs, std::string fs)
	: PostLight(sceneName, vs, fs, -1)
{
}

// creates a scene with a vertex shader and fragment shader, along with an index.
icg::PostLight::PostLight(std::string sceneName, std::string vs, std::string fs, int index)
	: index(index)
{
	using namespace cherry;

	// makes the shader
	shader = std::make_shared<Shader>();
	shader->Load(vs.c_str(), fs.c_str());
	
	// makes the mateiral
	material = std::make_shared<Material>(shader);

	CreateBodies(sceneName);
}

// constructor with shader.
icg::PostLight::PostLight(std::string sceneName, cherry::Shader::Sptr shader)
	: PostLight(sceneName, shader, -1)
{
}

// constructor with shader and index
icg::PostLight::PostLight(std::string sceneName, cherry::Shader::Sptr shader, int index)
	: shader(shader), index(index)
{
	using namespace cherry;

	// creates the material
	material = std::make_shared<Material>(shader);

	CreateBodies(sceneName);
}

// constructor with material.
icg::PostLight::PostLight(std::string sceneName, cherry::Material::Sptr material)
	: PostLight(sceneName, material, -1)
{
}

// constructor with shader and index
icg::PostLight::PostLight(std::string sceneName, cherry::Material::Sptr material, int index)
	: material(material), index(index)
{
	// saving the shader.
	shader = material->GetShader();

	// creates the bodies
	CreateBodies(sceneName);
}

// destructor
icg::PostLight::~PostLight()
{
	using namespace cherry;

	// removing the bodies
	ObjectManager::DeleteObjectFromSceneObjectList(bodies[0]);
	ObjectManager::DeleteObjectFromSceneObjectList(bodies[1]);
	ObjectManager::DeleteObjectFromSceneObjectList(bodies[2]);
}

// gets the sphere body
cherry::PrimitiveUVSphere* icg::PostLight::GetPrimitiveSphere() const
{
	return (cherry::PrimitiveUVSphere*)bodies[0];
}

// gets the cube body
cherry::PrimitiveCube* icg::PostLight::GetPrimitiveCube() const
{
	return (cherry::PrimitiveCube*)bodies[1];
}

// gets the cone body
cherry::PrimitiveCone* icg::PostLight::GetPrimitiveCone() const
{
	return (cherry::PrimitiveCone*)bodies[2];
}

// sets the volume type
void icg::PostLight::SetVolumeType(unsigned int index)
{
	// index out of bounds, or bodies don't exist
	if (index > BODY_COUNT || !bodiesCreated)
		return;

	// checking which body should be enabled.
	switch (index)
	{
	case 0: // sphere enabled
		bodies[0]->SetVisible(true);
		bodies[1]->SetVisible(false);
		bodies[2]->SetVisible(false);
		break;

	case 1: // cube enabled
		bodies[0]->SetVisible(false);
		bodies[1]->SetVisible(true);
		bodies[2]->SetVisible(false);
		break;

	case 2: // cone enabled
		bodies[0]->SetVisible(false);
		bodies[1]->SetVisible(false);
		bodies[2]->SetVisible(true);
		break;
	}
}

// gets the alpha value of the light volumes.
float icg::PostLight::GetAlpha() const
{
	if (!bodiesCreated) // bodies need to already exist.
		return -1.0F;

	// all the bodies have the same alpha value.
	bodies[0]->GetAlpha();
}

// sets the alpha for this volume
void icg::PostLight::SetAlpha(float alpha)
{
	if (!bodiesCreated) // bodies need to already exist.
		return;

	// sets the alpha (these already account for clamping)
	// although these save the same material, they all 
	bodies[0]->SetAlpha(alpha);
	bodies[1]->SetAlpha(alpha);
	bodies[2]->SetAlpha(alpha);
}

// creates the bodies
void icg::PostLight::CreateBodies(const std::string sceneName)
{
	using namespace cherry;

	// TODO: change size based on light travel distance

	// obejct list
	ObjectList* objectList = ObjectManager::GetSceneObjectListByName(sceneName);
	Vec4 color(1.0F, 1.0F, 1.0F, 1.0F); // the colour of the vertices.
	float alpha = 0.4F; // the alpha is a uniform float in the shader.

	// shader and material for the volumes.
	Shader::Sptr volShader;
	Material::Sptr volMaterial;

	volShader = std::make_shared<Shader>();
	volShader->Load("res/shaders/shader.vs.glsl", "res/shaders/shader.fs.glsl");
	
	volMaterial = std::make_shared<Material>(volShader);

	// sphere body
	bodies[0] = new PrimitiveUVSphere(12.0F, 15, 15, color);
	bodies[0]->CreateEntity(sceneName, volMaterial);

	bodies[0]->SetWireframeMode(true);
	bodies[0]->SetPostProcess(false);
	bodies[0]->SetAlpha(alpha);
	bodies[0]->SetVisible(true);

	bodies[0]->SetPosition(position);
	objectList->AddObject(bodies[0]);

	// cube body
	bodies[1] = new PrimitiveCube(12.0F, 12.0F, 12.0F, color);
	bodies[1]->CreateEntity(sceneName, volMaterial);

	bodies[1]->SetWireframeMode(true);
	bodies[1]->SetPostProcess(false);
	bodies[1]->SetAlpha(alpha);
	bodies[1]->SetVisible(false);

	bodies[1]->SetPosition(position);
	objectList->AddObject(bodies[1]);

	// cone body
	bodies[2] = new PrimitiveCone(12.0F, 18.0F, 15, color);
	bodies[2]->CreateEntity(sceneName, volMaterial);

	bodies[2]->SetWireframeMode(true);
	bodies[2]->SetPostProcess(false);
	bodies[2]->SetAlpha(alpha);
	bodies[2]->SetVisible(false);

	bodies[2]->SetPosition(position);
	objectList->AddObject(bodies[2]);

	// the bodies have been created.
	bodiesCreated = true;
}


// update
void icg::PostLight::Update(float deltaTime)
{
	// the update on the bodies is called automatically, so they aren't called here.
	using namespace cherry;

	// new position from path
	Vec3 newPos = path.Run(deltaTime);
	position = glm::vec3(newPos.v.x, newPos.v.y, newPos.v.z);
	
	// shader->SetUniform(("a_LightPos" + std::to_string(index)).c_str(), position);

	// changing position
	if (pvtPosition != position)
	{
		(index >= 0) ?
			shader->SetUniform(("a_Lights[" + std::to_string(index) + "].position").c_str(), position) : 
			shader->SetUniform("a_LightPos", position);

		pvtPosition = position;

		// updating body positions
		if (bodiesCreated)
		{
			bodies[0]->SetPosition(position);
			bodies[1]->SetPosition(position);
			bodies[2]->SetPosition(position);
		}
	}

	// changing color
	if (pvtColor != color)
	{
		(index >= 0) ?
			shader->SetUniform(("a_Lights[" + std::to_string(index) + "].color").c_str(), color) :
			shader->SetUniform("a_Color", color);

		pvtColor = color;
	}

	// changing attenuation
	if (pvtAttenuation != attenuation)
	{
		(index >= 0) ?
			shader->SetUniform(("a_Lights[" + std::to_string(index) + "].attenuation").c_str(), attenuation) :
			shader->SetUniform("a_Attenuation", color);

		pvtAttenuation = attenuation;
	}

	// changing shininess
	if (pvtShininess != shininess)
	{
		(index >= 0) ?
			shader->SetUniform(("a_Lights[" + std::to_string(index) + "].shininess").c_str(), shininess) :
			shader->SetUniform("a_MatShininess", shininess);
	
		pvtShininess = shininess;
	}
}