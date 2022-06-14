#pragma once

#include <glm/glm.hpp>

struct DirectionalLight 
{
	DirectionalLight() {
		enabled = false;
	}

	/*!
	 * Directional light constructor
	 * A light that gets emitted in a specific direction.
	 * @param color: color of the light
	 * @param direction: direction of the light
	 * @param enabled: if the light is enabled
	 */
	DirectionalLight(glm::vec3 color, glm::vec3 direction, bool enabled = true)
		: color(color), direction(glm::normalize(direction)), enabled(enabled)
	{}

	bool enabled;
	glm::vec3 color;
	glm::vec3 direction;
};

/*!
 * Point light, a light that gets emitted from a single point in all directions
 */
struct PointLight {

	PointLight() {
		enabled = false;
	}

	/*!
	 * Point light constructor
	 * A light that gets emitted from a single point in all directions
	 * @param color: color of the light
	 * @param position: position of the light
	 * @param attenuation: the light's attenuation (x = constant, y = linear, z = quadratic)
	 * @param enabled: if the light is enabled
	 */
	PointLight(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation, bool enabled = true)
		: color(color), position(position), attenuation(attenuation), enabled(enabled)
	{}

	bool enabled;
	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 attenuation; // The light's attenuation (x = constant, y = linear, z = quadratic)
};