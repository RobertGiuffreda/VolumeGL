#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Camera.h"

class Window {
public:
	Window(int height, int width);

	void processMouseMovement();
	void processKeyboard(Camera_Movement direction);
	glm::mat4 getViewMatrix();
	glm::mat4 getPerspectiveMatrix();
	glm::vec3 getCamPosition();
	void setCamPosition(glm::vec3 position); // Used??
	float getDeltaTime();
	glm::mat4 calcMVP();

	void process_input();
	void start();
	void end();
	int windowShouldClose();

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void size_callback(GLFWwindow* window, int width, int height);
private:
	GLFWwindow* handle;
	Camera cam;

	bool firstMouse;
	bool changed;
	float px, py;
	float dx, dy;

	float last_frame;
	float delta_time;

	glm::mat4 perspective;
};