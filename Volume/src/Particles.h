#pragma once

#include <glm.hpp>
#include <vector>
#include <iostream>
#include "Random.h"

struct particle {
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 mask;
};

enum class Layout
{
	CENTER,
	SHELL,
	SPHERE,
	CUBE
};

enum class Direction
{
	IN,
	OUT,
	RANDOM
};

class Particles {
public:
	Particles(glm::vec3 dim, size_t p_num, size_t g_num, Layout distribution, Direction direction);
	std::vector<particle> get_particles();
private:
	size_t p_num;
	size_t g_num;
	std::vector<particle> particles;
};