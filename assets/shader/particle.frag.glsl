#version 430 core

in vec2 UV;
vec4 colorIn = vec4(0.46 ,0.73, 1, 0.5);

out vec4 color;

layout(binding = 0) uniform sampler2D myTextureSampler;

void main(){

	color = texture( myTextureSampler, UV ) * colorIn;

}