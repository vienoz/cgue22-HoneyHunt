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

layout (location = 11) uniform vec3 camera_world;

layout (location = 8) uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 
layout (location = 9) uniform float specularAlpha;
layout (location = 10) uniform sampler2D diffuseTexture;


layout (location =14) uniform vec3 dirL_color;
layout (location =15) uniform vec3 dirL_direction;



layout (location =4) uniform vec3 pointL_color;
layout (location =5) uniform vec3 pointL_position;
layout (location =7) uniform vec3 pointL_attenuation; 



vec3 phong(vec3 n, vec3 l, vec3 v, vec3 diffuseC, float diffuseF, vec3 specularC, float specularF, float alpha, bool attenuate, vec3 attenuation) {
	float d = length(l);
	l = normalize(l);
	float att = 1.0;	
	if(attenuate) att = 1.0f / (attenuation.x + d * attenuation.y + d * d * attenuation.z);
	vec3 r = reflect(-l, n);
	return (diffuseF * diffuseC * max(0, dot(n, l)) + specularF * specularC * pow(max(0, dot(r, v)), alpha)) * att; 
}

void main() {	
	vec3 n = normalize(vert.normal_world);
	vec3 v = normalize(camera_world - vert.position_world);
	
	vec3 texColor = texture(diffuseTexture, vert.uv).rgb;
	color = vec4(texColor * materialCoefficients.x, 1); // ambient
	
	// add directional light contribution
	color.rgb += phong(n, -dirL_direction, v, dirL_color * texColor, materialCoefficients.y, dirL_color, materialCoefficients.z, specularAlpha, false, vec3(0));
			
	// add point light contribution
	color.rgb += phong(n, pointL_position - vert.position_world, v, pointL_color * texColor, materialCoefficients.y, pointL_color, materialCoefficients.z, specularAlpha, true, pointL_attenuation);

	//color= vec4(camera_world, 0.0f);
}