#version 430 core
/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

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

void main() {
	vert.normal_world = mat3(transpose(inverse(modelMatrix))) * normal;
	vert.uv = uv;
	vec4 position_world_ = modelMatrix * vec4(position, 1);
	vert.position_world = position_world_.xyz;
	gl_Position = projMatrix * viewMatrix * position_world_;
}