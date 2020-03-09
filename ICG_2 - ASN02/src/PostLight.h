/*
 * Name: Bonus Fruit
	- Kennedy Adams	(100632983)
	- Jonah Griffin	(100702748)
	- Nathan Tuck	(100708651)
	- Stephane Gagnon (100694227)
	- Roderick "R.J." Montague (100701758)
 * Date: 03/08/2020
 * Description: post processing light class.
 * References:
	* http://www.cplusplus.com/reference/vector/vector/resize/
*/

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
		PostLight() = default;

		// constructor - makes empty shader
		PostLight(std::string sceneName);

		// creates the post light by providing it with its vertex shader and fragment shader
		PostLight(std::string sceneName, std::string vs, std::string fs);

		// creates the post light by providing it with its vertex shader, fragement shader, and index in both.
		PostLight(std::string sceneName, std::string vs, std::string fs, int index);

		// constructor with shader
		PostLight(std::string sceneName, cherry::Shader::Sptr shader);

		// creates a light with a shader and index.
		// if the index is negative, it's assumed that its a single-light shader.
		PostLight(std::string sceneName, cherry::Shader::Sptr shader, int index);

		// constructor with material
		PostLight(std::string sceneName, cherry::Material::Sptr material);

		// creates a light with a material and index.
		// if the index is negative, it's assumed that its a single-light shader.
		PostLight(std::string sceneName, cherry::Material::Sptr material, int index);

		// destructor
		~PostLight();

		// gets the sphere body
		cherry::PrimitiveUVSphere* GetPrimitiveSphere() const;

		// gets the cube body
		cherry::PrimitiveCube* GetPrimitiveCube() const;

		// gets the cone body
		cherry::PrimitiveCone* GetPrimitiveCone() const;

		// gets the amount of volume types;
		static int GetVolumeTypeCount();

		// gets the volume index.
		int GetVolumeType() const;

		// sets the volume type being used.
		// 0 - sphere, 1 - cube, 2 - cone
		void SetVolumeType(unsigned int index);

		// gets the alpha value of the light volume.
		// if the bodies don't exist, -1 is returned.
		float GetAlpha() const;

		// sets the alpha of the volumes for this light.
		void SetAlpha(float alpha);

		// returns 'true' if a volume is visible.
		bool IsVisible() const;

		// turns on (or off) volume visibility
		void SetVisible(bool visible);

		// updates the light, updating the values in the shader for the light.
		// this also runs the path if there is one.
		void Update(float deltaTime);

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

		// shader
		// single - blinn-phong-post.fs.glsl
		// multi - blinn-phong-post-multi.fs.glsl
		cherry::Shader::Sptr shader = nullptr;

		// material
		cherry::Material::Sptr material = nullptr;

	private:

		// creates the bodies
		void CreateBodies(const std::string sceneName);

		// original values
		// the private versions are checked against the public versions.
		glm::vec3 pvtPosition{};
		glm::vec3 pvtColor{};

		float pvtAttenuation = 0.0F;
		float pvtShininess = 0.0F;

		// the amount of bodies avaialble
		static const int BODY_COUNT;

		/*
		 * 	the three bodies that can be swapped between.
		 *** 0 - sphere
		 *** 1 - cube
		 *** 2 - cone
		*/
		cherry::Primitive* bodies[3]{ nullptr, nullptr, nullptr };
		
		// enum for the active volume
		enum volume{
			none = 0, 
			sphere = 1,
			cube = 2,
			cone = 3
		};
		
		// the active volume
		volume activeVolume = none;

		// if 'true', then the bodies have been defined.
		bool bodiesCreated = false;

	protected:

	};
}