#pragma once


#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


class Camera {
public:
	Camera();
	glm::mat4 getViewMatrix() const;
	void move(glm::vec3 displacement);
	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();
	void mouseLook(glm::vec2 mouseDelta);
	void update();

	glm::vec3 position;
	glm::vec3 view;
	glm::vec3 up;
	glm::mat4 viewMatrix;

	bool hasAngleLock;
	float horizontalAngle;
	float verticalAngle;

	const float defaultSpeed = 0.05;
	float speed;
};