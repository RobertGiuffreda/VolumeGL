#include <iostream>
#include "Window.h"

Window::Window(int height, int width) : enter(true), changed(true), cursor(false), px(0),
										py(0), dx(0), dy(0), delta_time(0), last_frame(0)
{
	if (!glfwInit())
	{
		std::cout << "GLFW initialization failed!" << std::endl;
		exit(1);
	}

	handle = glfwCreateWindow(width, height, "Particle Viewer", nullptr, nullptr);
	if (!handle)
	{
		std::cout << "Particle viewer context creation failed." << std::endl;
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(handle);

	glfwSetWindowUserPointer(handle, this);

	auto size_call = [](GLFWwindow* w, int width, int height)
	{
		static_cast<Window*>(glfwGetWindowUserPointer(w))->size_callback(w, width, height);
	};
	auto mouse_call = [](GLFWwindow* w, double x, double y)
	{
		static_cast<Window*>(glfwGetWindowUserPointer(w))->mouse_callback(w, x, y);
	};

	glfwSetFramebufferSizeCallback(handle, size_call);
	glfwSetCursorPosCallback(handle, mouse_call);
	glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		glfwTerminate();
		exit(1);
	}
	glfwSwapInterval(1);

	cam = Camera(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	perspective = glm::perspective(45.0f, (float)width / height, 0.1f, 1000.0f);
}

Window::~Window() {}

int Window::windowShouldClose()
{
	return glfwWindowShouldClose(handle);
}

void Window::start()
{
	float curr_frame = (float)glfwGetTime();
	delta_time = curr_frame - last_frame;
	last_frame = curr_frame;

	process_input();

	if (changed) {
		cam.ProcessMouseMovement(dx, dy);
		changed = false;
	}
}

void Window::end()
{
	glfwSwapBuffers(handle);
	glfwPollEvents();
}

GLFWwindow* Window::getWindowHandle()
{
	return handle;
}

glm::vec3 Window::getCamPosition()
{
	return cam.pos;
}

float Window::getDeltaTime()
{
	return delta_time;
}

glm::mat4 Window::calcMVP()
{
	return perspective * cam.GetViewMatrix();
}

void Window::toggleCursor()
{
	if (cursor) {
		cursor = false;
		cam.Enable();
		enter = true;
		glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	} else {
		cam.Disable();
		cursor = true;
		glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Window::process_input()
{
	if (glfwGetKey(handle, GLFW_KEY_ESCAPE) == GLFW_PRESS && !cursor)
		toggleCursor();
	else if(glfwGetKey(handle, GLFW_KEY_ENTER) == GLFW_PRESS && cursor)
		toggleCursor();
	if (glfwGetKey(handle, GLFW_KEY_W) == GLFW_PRESS)
		processKeyboard(Camera_Movement::FORWARD);
	if (glfwGetKey(handle, GLFW_KEY_S) == GLFW_PRESS)
		processKeyboard(Camera_Movement::BACKWARD);
	if (glfwGetKey(handle, GLFW_KEY_A) == GLFW_PRESS)
		processKeyboard(Camera_Movement::LEFT);
	if (glfwGetKey(handle, GLFW_KEY_D) == GLFW_PRESS)
		processKeyboard(Camera_Movement::RIGHT);
}

void Window::processKeyboard(Camera_Movement direction)
{
	cam.ProcessKeyboard(direction, delta_time);
}

void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (enter)
	{
		px = xpos;
		py = ypos;
		enter = false;
		changed = false;
	}

	dx = xpos - px;
	dy = py - ypos; // reversed since y-coordinates go from bottom to top

	px = xpos;
	py = ypos;
	changed = true;
}

void Window::size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	perspective = glm::perspective(45.0f, (float)width / height, 0.1f, 1000.0f);
}