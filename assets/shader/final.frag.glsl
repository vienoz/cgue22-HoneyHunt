#version 430 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding = 0) uniform sampler2D screenTexture;
layout(binding = 1) uniform sampler2D brightnessTexture;

void main(){
    vec4 sceneColor = texture(screenTexture, texCoords);
    vec4 highlightColor = texture(brightnessTextures, texCoords);    
    FragColor = sceneColor + highlightColor;
}