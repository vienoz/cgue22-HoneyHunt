#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

out VertexData {
	vec3 position_world;
	vec3 normal_world;
	vec2 uv;
} vert;

layout(location = 0) uniform mat4 modelMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 projMatrix;

layout(location = 3) uniform vec3 cameraWorld;

layout(location = 4) uniform vec3 dirLColor;
layout(location = 5) uniform vec3 dirLDirection;

void main() {
	vert.normal_world = mat3(transpose(inverse(modelMatrix))) * normal;
	vert.uv = uv;
	vec4 position_world_ = modelMatrix * vec4(position, 1.0f);
	vert.position_world = position_world_.xyz;

	gl_Position = projMatrix * viewMatrix * position_world_;
}