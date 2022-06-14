#version 430 core

in VertexData {
	vec3 positionWorld;
	vec3 normalWorld;
	vec2 uv;
} vert;

in vec3 unitToCameraVector;

out vec4 color;

layout(binding = 0) uniform sampler2D diffuseTexture;

layout(location = 3) uniform vec3 cameraWorld;

layout(location = 4) uniform vec3 dirLColor;
layout(location = 5) uniform vec3 dirLDirection;

layout(location = 6) uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 
layout(location = 7) uniform float specularAlpha;

const float levels = 5.0;
const vec3 ambientLight = vec3(0.1);

void main() {
    float lightBrightness = clamp(dot(-dirLDirection, vert.normalWorld), 0.0, 1.0);
    lightBrightness = floor(lightBrightness * levels) / levels;

    float specularFactor = clamp(dot(reflect(-dirLDirection, vert.normalWorld), unitToCameraVector), 0.0, 1.0);
    specularFactor = floor(specularFactor * levels) / levels;

    vec4 textureColor = texture2D(diffuseTexture, vert.uv);

    vec3 finalDiffuse = dirLColor * lightBrightness;
    vec3 finalSpecular = dirLColor * specularFactor;

    color = vec4(textureColor.rgb * finalDiffuse + finalSpecular + ambientLight, textureColor.a);
}