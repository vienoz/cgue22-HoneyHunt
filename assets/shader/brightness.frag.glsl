#version 430 core

out vec4 oColor;
in vec2 texCoords;

layout(binding = 0) uniform sampler2D screenTexture;

void main()
{
    vec4 color = texture(screenTexture, texCoords);
    float brightness = color.r * 0.2126 + color.g *  0.7152 + color.b * 0.0722;
    
    if (brightness > 0.6)
        oColor = color * brightness; 
    else
        oColor = vec4(0, 0, 0, 1);
}