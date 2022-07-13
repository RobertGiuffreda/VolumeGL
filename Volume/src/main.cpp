#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <fstream>

#include "Particles.h"
#include "Simulation.h"

const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 1080;

const unsigned int NOISE_W = 1080;
const unsigned int NOISE_H = 1080;

//volume dimensions
const int XDIM = 128;
const int YDIM = 128;
const int ZDIM = 128;

const int PNUM = 262144;
/* 
262144 = 2^18
sqrt(262144) = 512
512/32 = 16
*/

const float move_dist = 1.0f;
const float sensor_dist = 5.0f;
const int sample_num = 20.0;
const float turn_speed = 0.5f;
const float deposit = 1.0f;

const float decay = 0.8f;
const float blur = 1.0f;

int main(void)
{
	Simulation_Settings s = {
		glm::vec2(SCR_WIDTH, SCR_HEIGHT),
		glm::vec3(XDIM, YDIM, ZDIM),

		PNUM,
		Layout::CENTER,
		Direction::RANDOM,

		move_dist,
		sensor_dist,
		sample_num,
		turn_speed,
		deposit,

		decay,
		blur,

		false
	};

	Simulation sim(s);
	sim.run();

	glfwTerminate();
	return 0;
}