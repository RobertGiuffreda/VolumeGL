#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm.hpp>
#include "Window.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "Marcher.h"
#include "FullScreenTexture.h"
#include "Particles.h"
#include "Shader.h"

struct Simulation_Settings {
	glm::vec2 window_dim;
	glm::vec3 dimensions;

	size_t pnum;
	int group_num;
	Layout distribution;
	Direction direction;

	int sample_num;
	float move_distance;
	float sensor_distance;
	float turn_speed;
	float deposit;

	float decay;
	float blur;

	bool dt;
};

class Simulation {
public:
	Window window;

	ImGuiIO *io;

	Particles particles;

	Texture initial_texture;
	Texture processed_texture;
	Texture display_texture;

	FullScreenTexture full_tex;
	Marcher marcher;

	ComputeShader particle_compute;
	ComputeShader process_compute;
	ComputeShader copy_compute;
	ComputeShader zero_compute;
	ComputeShader color_compute;

	Shader particle_shader;

	Simulation(Simulation_Settings s);
	~Simulation();

	void run();
private:
	unsigned int ssbo_id;
	unsigned int settings_ssbo;
	Simulation_Settings settings;
	bool dt;
};