#version 440

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inScreenCoords;
layout (location = 3) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D s_CameraDepth; // Camera's depth buffer
layout(binding = 2) uniform sampler2D s_GNormal;     // The normal buffer

uniform vec3 a_CameraPos; // camera position


// rendered image
uniform sampler2D xImage;

void main() {
	// getting the image
	vec4 img = texture(xImage, inUV);
	
	// gets the normal
	vec4 norm = texture(s_GNormal, inUV);
	norm.a = 1.0F;

	// getting the out color
	outColor = img * normalize(norm);
}