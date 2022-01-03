#pragma once
#include <glm.hpp>

#include "ComputeShader.h"
#include "Texture.h"
#include "Marcher.h"
#include "FullScreenTexture.h"
#include "Window.h"
#include "Particles.h"
#include "Shader.h"

struct Simulation_Settings {
	glm::vec2 window_dim;
	glm::vec3 dimensions;

	size_t pnum;
	Layout distribution;
	Direction direction;

	float move_distance;
	float sensor_distance;
	int sample_num;
	float turn_speed;

	float decay;
	float blur;
};

class Simulation {
public:
	Window window;

	Particles particles;

	Texture initial_texture;
	Texture processed_texture;

	FullScreenTexture full_tex;
	Marcher marcher;

	ComputeShader particle_compute;
	ComputeShader process_compute;
	ComputeShader copy_compute;
	ComputeShader zero_compute;

	Shader particle_shader;

	Simulation(Simulation_Settings s);
	~Simulation();

	void run();
private:
	unsigned int ssbo_id;
	Simulation_Settings settings;
};