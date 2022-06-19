#include "Simulation.h"

Simulation::Simulation(Simulation_Settings s) : 
	window(s.window_dim.x, s.window_dim.y),
	particles(s.dimensions, s.pnum, s.distribution, s.direction),
	initial_texture(s.dimensions.x, s.dimensions.y, s.dimensions.z, 1, 1),
	processed_texture(s.dimensions.x, s.dimensions.y, s.dimensions.z, 2, 2),
	full_tex(0),
	marcher(1, glm::vec3(1./ s.dimensions.x, 1./ s.dimensions.y, 1./ s.dimensions.z)),
	particle_compute("res/shaders/p_update.shader"),
	process_compute("res/shaders/blur_trail.shader"),
	copy_compute("res/shaders/copy.shader"),
	zero_compute("res/shaders/whiteblock.shader"),
	particle_shader("res/shaders/particle.vert", "res/shaders/particle.frag")
{
	glGenBuffers(1, &ssbo_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(particle) * s.pnum, (void*)&particles.get_particles()[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	settings = s;
}

Simulation::~Simulation()
{
	glDeleteBuffers(1, &ssbo_id);
}

void Simulation::run()
{
	while (!window.windowShouldClose())
	{
		window.start();

		process_compute.Bind();
		process_compute.SetUniform1f("decay_rate", settings.decay);
		process_compute.SetUniform1f("blur_factor", settings.blur);
		process_compute.SetUniform1f("delta_time", window.getDeltaTime());
		//process_compute.SetUniform1f("delta_time", 0.7f);
		process_compute.SetUniformVec3("dim", settings.dimensions);
		glDispatchCompute(settings.dimensions.x, settings.dimensions.y, settings.dimensions.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		copy_compute.Bind();
		glDispatchCompute(settings.dimensions.x, settings.dimensions.y, settings.dimensions.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		particle_compute.Bind();
		particle_compute.SetUniformVec3("dim", settings.dimensions);
		particle_compute.SetUniform1f("time", glfwGetTime());
		//particle_compute.SetUniform1f("delta_time", window.getDeltaTime());
		particle_compute.SetUniform1f("delta_time", 0.9f);
		particle_compute.SetUniform1f("turn_speed", settings.turn_speed);
		particle_compute.SetUniform1i("sample_num", settings.sample_num);
		particle_compute.SetUniform1f("sensor_dist", settings.sensor_distance);
		particle_compute.SetUniform1f("move_dist", settings.move_distance);

		glDispatchCompute(settings.pnum, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glm::vec3 camPos = window.getCamPosition();
		glm::mat4 MVP = window.calcMVP();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//marcher.render(MVP, camPos);

		//full_tex.render();
		
		particle_shader.Bind();
		particle_shader.SetUniformMat4("MVP", MVP);
		particle_shader.SetUniformVec3("dim", settings.dimensions);
		glDrawArrays(GL_POINTS, 0, settings.pnum);

		window.end();
	}
}