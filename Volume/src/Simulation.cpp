#include <iostream>
#include "Simulation.h"

Simulation::Simulation(Simulation_Settings s) : 
	window(s.window_dim.x, s.window_dim.y),
	particles(s.dimensions, s.pnum, s.group_num, s.distribution, s.direction),
	initial_texture(s.dimensions.x, s.dimensions.y, s.dimensions.z, 1, 1),
	processed_texture(s.dimensions.x, s.dimensions.y, s.dimensions.z, 2, 2),
	display_texture(s.dimensions.x, s.dimensions.y, s.dimensions.z, 3, 3),
	full_tex(0),
	marcher(3, glm::vec3(1./ s.dimensions.x, 1./ s.dimensions.y, 1./ s.dimensions.z)),
	particle_compute("res/shaders/p_update.shader"),
	process_compute("res/shaders/blur_trail.shader"),
	copy_compute("res/shaders/copy.shader"),
	zero_compute("res/shaders/whiteblock.shader"),
	color_compute("res/shaders/color.shader"),
	particle_shader("res/shaders/particle.vert", "res/shaders/particle.frag"),
	dt(s.dt)
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

	/* Tempryary group settings code/struct */

	struct group_settings {
		glm::vec4 color;
	};

	std::vector<group_settings> gs;
	gs.resize(s.group_num);
	for (int i = 0; i < s.group_num; ++i)
		gs[i].color = glm::vec4(i == 0, i == 1, i == 2, 1.0f);

	for (int i = 0; i < s.group_num; ++i)
		std::cout << "r: " << gs[i].color.r << "  g: " << gs[i].color.g << "  b: " << gs[i].color.b << "  a: " << gs[i].color.a << std::endl;

	glGenBuffers(1, &settings_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, settings_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct group_settings) * s.group_num, (void*)&gs[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, settings_ssbo);
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

		particle_compute.Bind();
		particle_compute.SetUniformVec3("dim", settings.dimensions);

		particle_compute.SetUniform1f("move_dist", settings.move_distance);
		particle_compute.SetUniform1f("sensor_dist", settings.sensor_distance);
		particle_compute.SetUniform1i("sample_num", settings.sample_num);
		particle_compute.SetUniform1f("turn_speed", settings.turn_speed);
		particle_compute.SetUniform1f("sensor_dist", settings.sensor_distance);
		particle_compute.SetUniform1f("deposit", settings.deposit);

		particle_compute.SetUniform1f("time", glfwGetTime());
		particle_compute.SetUniform1f("delta_time", (dt) ? window.getDeltaTime() : 1.0f);

		glDispatchCompute(settings.pnum/1024, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		process_compute.Bind();
		process_compute.SetUniformVec3("dim", settings.dimensions);
		process_compute.SetUniform1f("decay", settings.decay);
		process_compute.SetUniform1f("blur", settings.blur);
		process_compute.SetUniform1f("delta_time", (dt) ? window.getDeltaTime() : 1.0f);
		glDispatchCompute(settings.dimensions.x / 8, settings.dimensions.y / 8, settings.dimensions.z / 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		copy_compute.Bind();
		glDispatchCompute(settings.dimensions.x / 8, settings.dimensions.y / 8, settings.dimensions.z / 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		color_compute.Bind();
		color_compute.SetUniform1i("group_num", settings.group_num);
		glDispatchCompute(settings.dimensions.x / 8, settings.dimensions.y / 8, settings.dimensions.z / 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

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
		ImGui::SliderFloat("Speed", &(settings.move_distance), 0.0f, 10.0f);
		ImGui::SliderFloat("Sensor Range", &(settings.sensor_distance), 0.0f, 10.0f);
		ImGui::Checkbox("Delta Time", &dt);
		ImGui::SliderFloat("Turn Speed", &(settings.turn_speed), -3.0f, 3.0f);
		ImGui::SliderFloat("Decay", &(settings.decay), 0.0f, 1.0f);
		ImGui::SliderFloat("Blur", &(settings.blur), 0.0f, 1.0f);
		ImGui::SliderFloat("Deposit", &(settings.deposit), 0.0f, 5.0f);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.end();
	}
}