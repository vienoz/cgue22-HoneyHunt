#version 430 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding = 0) uniform sampler2D screenTexture;
layout(binding = 1) uniform sampler2D brightnessTexture;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

vec3 blurHorizontal(sampler2D tex, vec2 uv){
    vec2 tex_offset = 1.0 / textureSize(tex, 0); // gets size of single texel
    vec3 result = texture(tex, uv).rgb * weight[0]; // current fragment's contribution
    
    
    float intensity = 1.5;
    for(int i = 1; i < 15; ++i)
    {
        result += texture(tex, uv + vec2(tex_offset.x * i * intensity, 0.0)).rgb * weight[i/3];
        result += texture(tex, uv - vec2(tex_offset.x * i * intensity, 0.0)).rgb * weight[i/3];
    }
    for(int i = 1; i < 15; ++i)
    {
        result += texture(tex, uv + vec2(0.0, tex_offset.y * i * intensity)).rgb * weight[i/3];
        result += texture(tex, uv - vec2(0.0, tex_offset.y * i * intensity)).rgb * weight[i/3];
    }

    return result;
}

void main(){
    vec4 sceneColor = texture(screenTexture, texCoords);
    vec3 highlightColor = blurHorizontal(brightnessTexture, texCoords);    
    
    float emissionMultiplier = 0.6;
    FragColor = sceneColor + vec4(highlightColor, 0) * emissionMultiplier;
}

