#version 430 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding = 0) uniform sampler2D screenTexture;

void main(){
    //FragColor = texture(screenTexture, texCoords);
    //FragColor = vec4(1.0f) -  texture(screenTexture, texCoords);

    //FragColor = vec4(texCoords.x, texCoords.y, 0, 1);

    FragColor = texture(screenTexture, texCoords) * vec4(0.5, 1, 0, 1);
}