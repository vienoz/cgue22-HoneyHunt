#version 430 core
/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

in VertexData {
	vec3 position_world;
	vec3 normal_world;
	vec2 uv;
} vert;

out vec4 color;

layout (binding = 0) uniform sampler2D diffuseTexture;

void main() {	
	//color = vec4(vert.uv.x, vert.uv.y, 1, 1);
	color = texture2D(diffuseTexture, vert.uv);
}