#include "PostLight.h"

// default constructor.
icg::PostLight::PostLight()
{
	shader = std::make_shared<cherry::Shader>();
}

// constructor with shader.
icg::PostLight::PostLight(cherry::Shader::Sptr shader)
{
	this->shader = shader;
}

// update
void icg::PostLight::Update(float deltaTime)
{
	using namespace cherry;

	// new position from path
	Vec3 newPos = path.Run(deltaTime);
	position = glm::vec3(newPos.v.x, newPos.v.y, newPos.v.z);
	
	shader->SetUniform(("a_LightPos" + std::to_string(index)).c_str(), position);

	// changing position
	if (pvtPosition != position)
	{
		(index >= 0) ?
			shader->SetUniform(("a_PostLights[" + std::to_string(index) + "].position").c_str(), position) : 
			shader->SetUniform("a_LightPos", position);

		pvtPosition = position;
	}

	// changing color
	if (pvtColor != color)
	{
		(index >= 0) ?
			shader->SetUniform(("a_PostLights[" + std::to_string(index) + "].color").c_str(), color) :
			shader->SetUniform("a_Color", color);

		pvtColor = color;
	}

	// changing attenuation
	if (pvtAttenuation != attenuation)
	{
		(index >= 0) ?
			shader->SetUniform(("a_PostLights[" + std::to_string(index) + "].attenuation").c_str(), attenuation) :
			shader->SetUniform("a_Attenuation", color);

		pvtAttenuation = attenuation;
	}

	// changing shininess
	if (pvtShininess != shininess)
	{
		(index >= 0) ?
			shader->SetUniform(("a_PostLights[" + std::to_string(index) + "].shininess").c_str(), shininess) :
			shader->SetUniform("a_MatShininess", shininess);
	
		pvtShininess = shininess;
	}
}
