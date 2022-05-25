#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

out VertexData {
	vec3 positionWorld;
	vec3 normalWorld;
	vec2 uv;
} vert;

out vec3 unitToCameraVector;

layout(location = 0) uniform mat4 modelMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 projMatrix;

layout(location = 3) uniform vec3 cameraWorld;

void main() {
	//vert.positionWorld = mat3(transpose(inverse(modelMatrix))) * normal;
	vec4 globalPosition = modelMatrix * vec4(position, 1);

	vert.positionWorld = globalPosition.xyz;
	//vert.normalWorld = normalize(modelMatrix * vec4(normal, 1)).xyz;
	
	vert.normalWorld = normalize(transpose(inverse(mat3(modelMatrix))) * normal);
	//vert.normalWorld = normal;
	vert.uv = uv;

	unitToCameraVector = normalize(vert.positionWorld - cameraWorld);

	gl_Position = projMatrix * viewMatrix * globalPosition;
}