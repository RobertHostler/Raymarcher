#include "Camera.hpp"


Camera::Camera(){
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	view = glm::vec3(0.0f, 0.0f, 1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	viewMatrix = glm::lookAt(position, position + view, up);

	speed = defaultSpeed;

	horizontalAngle = 0.0f;
	verticalAngle = 0.0f;
	hasAngleLock = true;
}

glm::mat4 Camera::getViewMatrix() const {
	// return cached camera matrix
	return viewMatrix;
}

void Camera::move(glm::vec3 displacement){
	position += displacement;
}

void Camera::moveForward(){
	// This is for 'flying mode' (always moves along the camera view direction)
	position += view*speed;

	// for FPS mode (always moves horizontally, even when looking up or down)
	//position += glm::vec3(view.x, 0.0, view.z) * speed;
}

void Camera::moveBackward(){
	// This is for 'flying mode' (always moves along the camera view direction)
	position += -view*speed;

	// for FPS mode (always moves horizontally, even when looking up or down)
	//position += -glm::vec3(view.x, 0.0, view.z) * speed;
}

void Camera::moveLeft(){
	position += -glm::cross(view, up)*speed;
}

void Camera::moveRight(){
	position += glm::cross(view, up)*speed;
}

void Camera::mouseLook(glm::vec2 mouseDelta){
	// because we have to specify the angles directly, this *may or may not* be a true
	// quaternion camera. It is a matter of debate, but this *might* still have gimbal
	// lock, even though I have not seen that happen in practice.

	horizontalAngle += 0.01f*mouseDelta.x;
	verticalAngle -= 0.01f*mouseDelta.y;

	if(hasAngleLock){
		if(verticalAngle > M_PI/2.0f){
			verticalAngle = M_PI/2.0f;
		}
		if(verticalAngle < -M_PI/2.0f){
			verticalAngle = -M_PI/2.0f;
		}
	}

	// rotate the new view vector about the vertical axis
	glm::vec3 newUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 newView = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::quat p = glm::angleAxis(horizontalAngle, newUp);
	glm::quat p1 = glm::conjugate(p) * glm::quat(0.0f, newView.x, newView.y, newView.z) * p;

	// rotate the new view vector about the right axis (the camera's local x-axis)
	newView = glm::vec3(p1.x, p1.y, p1.z);
	glm::vec3 newRight = glm::cross(newUp, newView);
	glm::quat q = glm::angleAxis(verticalAngle, newRight);
	glm::quat q1 = glm::conjugate(q) * glm::quat(0.0f, newView.x, newView.y, newView.z) * q;
	newView = glm::vec3(q1.x, q1.y, q1.z);
	newUp = glm::cross(newView, newRight);

	// compute the new camera view matrix
	view = glm::vec3(newView.x, newView.y, newView.z);
	up = glm::vec3(newUp.x, newUp.y, newUp.z);
	viewMatrix = glm::lookAt(position, position + view, up);
}

void Camera::update(){
	viewMatrix = glm::lookAt(position, position + view, up);
}