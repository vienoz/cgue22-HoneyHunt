#version 430 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding = 0) uniform sampler2D screenTexture;

void main(){
    vec3 color = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}