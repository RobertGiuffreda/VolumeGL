#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Camera.h"

class Window {
public:
	Window(int height, int width);
	~Window();

	GLFWwindow* getWindowHandle();
	glm::vec3 getCamPosition();
	float getDeltaTime();
	glm::mat4 calcMVP();

	int windowShouldClose();
	void start();
	void end();

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void size_callback(GLFWwindow* window, int width, int height);

private:
	GLFWwindow* handle;
	Camera cam;

	bool enter;
	bool changed;
	bool cursor;

	float px, py;
	float dx, dy;

	float last_frame;
	float delta_time;

	glm::mat4 perspective;

	void toggleCursor();
	void process_input();
	void processKeyboard(Camera_Movement direction);
};