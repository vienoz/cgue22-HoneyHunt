#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
out vec2 UV;

// Values that stay constant for the whole mesh.^
layout(location = 0) uniform mat4 modelMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 projMatrix;
layout(location = 3) uniform vec3 cameraRight;
layout(location = 4) uniform vec3 cameraUp;

void main()
{
	float particleSize = xyzs.w; // because we encoded it this way.
	vec3 particleCenter_wordspace = xyzs.xyz;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ normalize(cameraRight) * squareVertices.x * particleSize
		+ normalize(cameraUp) * squareVertices.y * particleSize;

	vec4 pos = modelMatrix * vec4(particleCenter_wordspace, 1);
	// Output position of the vertex
	gl_Position = projMatrix * viewMatrix * vec4(vertexPosition_worldspace, 1);

	// UV of the vertex. No special space for this one.
	UV = squareVertices.xy + vec2(0.5, 0.5);
}