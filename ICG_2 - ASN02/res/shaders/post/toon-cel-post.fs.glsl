#version 440

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inScreenCoords;
layout (location = 3) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2D s_CameraDepth; // camera's depth buffer
layout (binding = 2) uniform sampler2D s_GNormal;     // normal buffer

// The position of the camera, in world space
uniform vec3 a_CameraPos;

// the image
uniform sampler2D xImage;

void main()
{
	// re-normalizing the normal so that it's of length 1.
	// vec3 norm = normalize(inNormal);

	vec3 screenPos = vec3(0, 0, 0);

	// position in the screen coordinates
	screenPos.x = inScreenCoords.x;
	screenPos.y = inScreenCoords.y;
	screenPos.z = texture(s_CameraDepth, inUV).r;
	
	// finding the pixel in reference to the camera. The closer the value is to 0, the closer it is to the edge.
	float edge = dot(normalize(a_CameraPos - screenPos.xyz), inNormal);

	// gets the pixel
	vec4 pixel = texture(xImage, inUV);

	// TODO: gamma correction
	// I did not have time to do it.

	// keeping 'edge' in bounds.
	edge = clamp(edge, 0, 1); // edge limits

	// if the edge is 
	// setting the colour for the edge, and writing output.
	// if(edge < a_RimSize)
	// {
	// 	outColor = vec4(result * (1 - edge), inColor.a); // light colour as edge colour
	// }
	// else
	// {
	// 	outColor = vec4(a_AmbientColor, 1); // regular ambient colour
	// }

	pixel *= vec4(edge, edge, edge, 1.0F);

	// does not work.
	// outline thickness
	// if it's a background pixel, it isn't counted.
	if(edge < 0.2F && texture(s_CameraDepth, inUV).r * 2 - 1 != 1.0F)
	{
		outColor = vec4(1.0F, 1.0F, 1.0F, 1.0F);
	}
	else{
		outColor = inColor;
	}

	outColor = texture(xImage, inUV);
}
