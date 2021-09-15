#include <iostream>
#include "Camera.h"
#include "gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 gaze, glm::vec3 wup, float yaw, float pitch)
	: pos(position), gaze(gaze), yaw(yaw), pitch(pitch),
	MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	worldUp = wup;
	UpdateCameraVectors();
}

Camera::Camera(float xpos, float ypos, float zpos, float xup, float yup, float zup, float yaw, float pitch)
	: gaze(glm::vec3(0.0f, 0.0f, -1.0f)), yaw(yaw), pitch(pitch), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	pos = glm::vec3(xpos, ypos, zpos);
	worldUp = glm::vec3(xup, yup, zup);
	UpdateCameraVectors();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{

	switch (direction)
	{
	case Camera_Movement::FORWARD:
		pos += gaze * MovementSpeed * deltaTime;
		break;
	case Camera_Movement::BACKWARD:
		pos -= gaze * MovementSpeed * deltaTime;
		break;
	case Camera_Movement::LEFT:
		pos -= right * MovementSpeed * deltaTime;
		break;
	case Camera_Movement::RIGHT:
		pos += right * MovementSpeed * deltaTime;
		break;
	}
}

void Camera::ProcessMouseMovement(float xoff, float yoff)
{
	yaw += xoff * MouseSensitivity;
	pitch += yoff * MouseSensitivity;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	UpdateCameraVectors();
}

void Camera::ProcessScroll(float offset)
{
	Zoom -= offset;
	if (Zoom < 1.0f) Zoom = 1.0f;
	if (Zoom > 45.0f) Zoom = 45.0f;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(pos, pos + gaze, up);
}

glm::mat3 Camera::MarchViewMatrix()
{
	return glm::mat3(right, up, gaze);
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	gaze = glm::normalize(front);

	right = glm::normalize(glm::cross(gaze, worldUp));
	up = glm::normalize(glm::cross(right, gaze));
}