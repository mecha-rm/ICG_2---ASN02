#pragma once
#include <glm/vec3.hpp>
#include "cherry/Shader.h"
#include "cherry/animate/Path.h"
#include "cherry/objects/Primitives.h"

// a class for post processed lights
namespace icg
{
	class PostLight
	{
	public:
		// constructor - makes empty shader
		PostLight();

		// constructor with shader
		PostLight(cherry::Shader::Sptr shader);

		// creates a light with a shader and index.
		// if the index is negative, it's assumed that its a single-light shader.
		PostLight(cherry::Shader::Sptr shader, int index);

		// updates the light, updating the values in the shader for the light.
		// this also runs the path if there is one.
		void Update(float deltaTime);

		// shader
		// single - blinn-phong-post.fs.glsl
		// multi - blinn-phong-post-multi.fs.glsl
		cherry::Shader::Sptr shader;

		// position
		glm::vec3 position{};

		// colour
		glm::vec3 color{};
		
		// attenuation
		float attenuation = 0.0F;
		
		// shininess
		float shininess = 0.0F;

		// path object for moving the light.
		cherry::Path path = cherry::Path();

		// the index of the light in the shader.
		// if the index is set to -1, then it is the only light for the shader.
		int index = -1;
	private:
		// the body for the object.
		cherry::PrimitiveUVSphere * body;

		// original values
		// the private versions are checked against the public versions.
		glm::vec3 pvtPosition;
		glm::vec3 pvtColor;
		float pvtAttenuation;
		float pvtShininess;

	protected:

	};
}