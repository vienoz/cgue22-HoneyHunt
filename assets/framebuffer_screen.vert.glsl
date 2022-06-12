#version 430 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoords;

layout(location = 4) uniform vec3 dirLColor;
layout(location = 5) uniform vec3 dirLDirection;

out vec2 TexCoords;

void main()
{
	TexCoords = inTexCoords;
	gl_Position vec4(inPos.x, inPos.y, 0.0, 1.0);
}