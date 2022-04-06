#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <iostream>

/*!
 * Arc ball camera, modified by mouse input
 */
class Camera
{
protected:
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;
	float _mouseX;
	float _mouseY;
	float _yaw, _pitch;
	glm::vec3 _position;
	glm::vec3 _target;

public:
	/*!
	 * Camera constructor
	 * @param fov: field of view, in degrees
	 * @param aspect: aspect ratio
	 * @param near: near plane
	 * @param far: far plane
	 */
	Camera(float fov, float aspect, float near, float far, glm::vec3 position, glm::vec3 target);

	~Camera();

	/*!
	 * @return the current position of the camera
	 */
	glm::vec3 getPosition();

	/*!
	 * @return the view-projection matrix
	 */
	glm::mat4 getViewProjectionMatrix();


	glm::mat4 getViewMatrix();

	glm::mat4 getProjMatrix();

	/*!
	 * Updates the camera's position and view matrix according to the input
	 * @param mouseX: current mouse x position
	 * @param mouseY: current mouse x position
	 * @param zoom: zoom multiplier
	 * @param dragging: is the camera dragging
	 */
	void update(double mouseX, double mouseY, float zoom, bool dragging);
};