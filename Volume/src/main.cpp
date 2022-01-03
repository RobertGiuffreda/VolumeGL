#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <fstream>

#include "Particles.h"
#include "Simulation.h"

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

const unsigned int NOISE_W = 1024;
const unsigned int NOISE_H = 1024;

//volume dimensions
const int XDIM = 64;
const int YDIM = 64;
const int ZDIM = 64;

const int PNUM = 200000;

const float move_dist = 1.0f;
const float sensor_dist = 5.0f;
const int sample_num = 20.0;
const float turn_speed = 0.5f;

const float decay = 0.05f;
const float blur_factor = 0.3f;

int main(void)
{
	Simulation_Settings s = {
		glm::vec2(SCR_WIDTH, SCR_HEIGHT),
		glm::vec3(XDIM, YDIM, ZDIM),

		PNUM,
		Layout::CUBE,
		Direction::RANDOM,

		move_dist,
		sensor_dist,
		sample_num,
		turn_speed,

		decay,
		blur_factor
	};

	Simulation sim(s);
	sim.run();

	glfwTerminate();
	return 0;
}