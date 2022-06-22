#version 430 core

layout(location = 0) in vec3 verts;
layout(location = 1) in vec4 xyzs; 

out vec2 UV;

layout(location = 0) uniform mat4 modelMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 projMatrix;
layout(location = 3) uniform vec3 cameraRight;
layout(location = 4) uniform vec3 cameraUp;

void main()
{
	float particleSize = xyzs.w; 
	vec3 particleCenter_wordspace = xyzs.xyz;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ normalize(cameraRight) * verts.x * particleSize
		+ normalize(cameraUp) * verts.y * particleSize;

	vec4 pos = modelMatrix * vec4(particleCenter_wordspace, 1);

	gl_Position = projMatrix * viewMatrix * vec4(vertexPosition_worldspace, 1);

	UV = verts.xy + vec2(0.5, 0.5);
}