#version 430 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoords;

layout(location = 4) uniform vec3 dirLColor;
layout(location = 5) uniform vec3 dirLDirection;

out vec2 TexCoords;

layout(location = 0) uniform mat4 modelMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 projMatrix;

void main()
{
	TexCoords = inTexCoords;
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(inPos.x, 1.0f);
}