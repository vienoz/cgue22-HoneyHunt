#version 430 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

void main(){
    //FragColor = vec4(1.0f) - texture(screenTexture, texCoords);
    FragColor = vec4(1.0f) -  texture(screenTexture, texCoords);
}