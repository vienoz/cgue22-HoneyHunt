#include "Camera.h"


Camera::Camera(float fov, float aspect, float near, float far, glm::vec3 position, glm::vec3 target) : _position(position), _target(target) {

	_projMatrix = glm::perspective(glm::radians(fov), aspect, near, far);
	_yaw = 0;
	_pitch = 0;
	_mouseX = 0;
	_mouseY = 0;

}

Camera::Camera(){}

Camera::~Camera() {
}

glm::vec3 Camera::getPosition() {
	return _position;
}

glm::mat4 Camera::getViewMatrix() {
	glm::vec3 zaxis = glm::normalize(_position - _target);                 //camerDirection
	glm::vec3 xaxis = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), zaxis));        // normal of global y and straight foward is cameraRight/ camera
	cameraX = xaxis;
	glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));     // normal of right and straight foward is cameraUp/cameraY


	glm::mat4 orientation = {
		glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0),
		glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0),
		glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0),
		glm::vec4(0,       0,       0,     1)
	};

	glm::mat4 translation = {
		glm::vec4(1,0,0,0),
		glm::vec4(0,1,0,0),
		glm::vec4(0,0,1,0),
		glm::vec4(-_position.x, -_position.y, -_position.z, 1)
	};

	return (orientation * translation);
}

glm::mat4 Camera::getProjMatrix() {
	return _projMatrix;
}

void Camera::setTarget(glm::vec3 newTarget) {
	_target = newTarget;
}

void Camera::update(double mouseX, double mouseY, float zoom, bool dragging, glm::vec3 newTarget) {
	setTarget(newTarget);
	float cameraDistance = (glm::length(_position - _target));


	if (dragging) {

		float xoffset = mouseX - _mouseX;
		float yoffset = _mouseY - mouseY;

		_mouseX = mouseX;
		_mouseY = mouseY;

		const float sensitivity = 0.5f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		_yaw += xoffset;
		_pitch += yoffset;


		//prevent inverted camera system
		if (_pitch > 89.0f)
			_pitch = 89.0f;
		if (_pitch < -89.0f)
			_pitch = -89.0f;


	}
	else {
		_mouseX = mouseX;
		_mouseY = mouseY;
	}
	_position.x = _target.x + zoom * -sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_position.y = _target.y + zoom * -sin(glm::radians(_pitch)); //negative to achieve expected y movement
	_position.z = _target.z + zoom * cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));

}

glm::vec3 Camera::getCameraRight() {
	return cameraX;
}

glm::vec3 Camera::getCameraFoward() {
	return glm::normalize(_position - _target);
	//return glm::normalize(glm::cross(cameraX, glm::vec3(0.0, 1.0, 0.0)));
}