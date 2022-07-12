#include <iostream>
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
	particle_shader("res/shaders/particle.vert", "res/shaders/particle.frag"),
	dt(true)
{
	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	std::cout << "Max work groups per compute shader" << std::endl <<
		"x: " << work_grp_cnt[0] << std::endl <<
		"y: " << work_grp_cnt[1] << std::endl <<
		"z: " << work_grp_cnt[2] << std::endl;

	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	std::cout << "Max work group sizes" << std::endl << 
		"x: " << work_grp_size[0] << std::endl <<
		"y: " << work_grp_size[1] << std::endl <<
		"z: " << work_grp_size[2] << std::endl;

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	std::cout << "Max invocations count per work group: " << work_grp_inv << std::endl;

	glGenBuffers(1, &ssbo_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(particle) * s.pnum, (void*)&particles.get_particles()[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	settings = s;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.getWindowHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

Simulation::~Simulation()
{
	glDeleteBuffers(1, &ssbo_id);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Simulation::run()
{
	while (!window.windowShouldClose())
	{
		window.start();

		// ImGui New Frame and creation of such
 		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		process_compute.Bind();
		process_compute.SetUniform1f("decay", settings.decay);
		process_compute.SetUniform1f("blur", settings.blur);
		process_compute.SetUniform1f("delta_time", window.getDeltaTime());
		//process_compute.SetUniform1f("delta_time", 0.7f);
		process_compute.SetUniformVec3("dim", settings.dimensions);
		glDispatchCompute(settings.dimensions.x/8, settings.dimensions.y/8, settings.dimensions.z/8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		copy_compute.Bind();
		glDispatchCompute(settings.dimensions.x/8, settings.dimensions.y/8, settings.dimensions.z/8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		particle_compute.Bind();
		particle_compute.SetUniformVec3("dim", settings.dimensions);
		particle_compute.SetUniform1f("time", glfwGetTime());
		particle_compute.SetUniform1f("delta_time", (dt) ? window.getDeltaTime() : 0.9f);
		particle_compute.SetUniform1f("turn_speed", settings.turn_speed);
		particle_compute.SetUniform1i("sample_num", settings.sample_num);
		particle_compute.SetUniform1f("sensor_dist", settings.sensor_distance);
		particle_compute.SetUniform1f("move_dist", settings.move_distance);

		glDispatchCompute(16, 16, 1);
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

		// ImGui
		ImGui::Begin("Hello.");
		ImGui::Text("Banano is it. It's nice to meet a man such as yourself!");
		ImGui::SliderFloat("Speed", &(settings.move_distance), 0.0f, 5.0f);
		ImGui::SliderFloat("Sensor Range", &(settings.sensor_distance), 3.0f, 10.0f);
		ImGui::Checkbox("Delta Time", &dt);
		ImGui::SliderFloat("Turn Speed", &(settings.turn_speed), 0.0f, 2.0f);
		ImGui::SliderFloat("Decay", &(settings.decay), 0.0f, 1.0f);
		ImGui::SliderFloat("Blur", &(settings.blur), 0.0f, 1.0f);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.end();
	}
}