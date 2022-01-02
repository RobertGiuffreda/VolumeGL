#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <fstream>

#include "ComputeShader.h"
#include "Shader.h"
#include "Camera.h"
#include "Random.h"
#include "Particles.h"

#include "Window.h"

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

//const unsigned int NOISE_W = 1024;
//const unsigned int NOISE_H = 1024;

//volume dimensions
const int XDIM = 128;
const int YDIM = 128;
const int ZDIM = 128;

const int PNUM = 200000;

const float move_dist = 1.0f;
const float sensor_dist = 5.0f;
const int sample_num = 20.0;
const float turn_speed = 0.5f;

const float decay = 0.2f;
const float blur_factor = 0.2f;

unsigned int cubeVBOID;
unsigned int cubeVAOID;
unsigned int cubeIndicesID;

//unsigned int noiseID;
unsigned int tex1ID;
unsigned int tex2ID;
unsigned int ssbo;

int main(void)
{
	Window window(SCR_HEIGHT, SCR_WIDTH);

	/* Vertices of polygon to display texture */
	float screen_poly[] = {
		 1.0f,  1.0f, 0.0f, 	1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 	1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 	0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 	0.0f, 1.0f,
	};

	unsigned int screen_indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	/* OpenGL data structures to display texture on screen */
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (5 * 4), screen_poly, GL_STATIC_DRAW);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, screen_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* Generate Gaussian distributed noise texture to sample in shaders */
	//std::vector<glm::vec2> noise;
	//for (int i = 0; i < NOISE_W * NOISE_H; i++) {
	//	glm::vec2 rand = glm::normalize(glm::vec2(standardNormal(), standardNormal()));
	//	noise.push_back(rand);
	//}

	//glGenTextures(1, &noiseID);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, noiseID);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, NOISE_W, NOISE_H, 0, GL_RG, GL_FLOAT, &noise[0]);

	/* Create particle vector */
	Particles particles(glm::vec3(XDIM, YDIM, ZDIM), (size_t)PNUM, Layout::SPHERE, Direction::RANDOM);

	/* Shader storage buffer object to contain all of the particles */
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(particle) * PNUM, (void *)&particles.get_particles()[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// 3D texture
	glGenTextures(1, &tex1ID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, tex1ID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, XDIM, YDIM, ZDIM, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(1, tex1ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenTextures(1, &tex2ID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, tex2ID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, XDIM, YDIM, ZDIM, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(2, tex2ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	ComputeShader update("res/shaders/p_update.shader");
	ComputeShader blur("res/shaders/blur_trail.shader");
	ComputeShader copy("res/shaders/copy.shader");
	ComputeShader white("res/shaders/whiteblock.shader");

	white.Bind();
	glDispatchCompute(XDIM, YDIM, ZDIM);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	/* Marcher to be cut from program for now */
	Shader marcher("res/shaders/marchervert.shader", "res/shaders/marcherfrag.shader");
	marcher.Bind();
	marcher.SetUniformVec3("step_size", glm::vec3(1.0f / XDIM, 1.0f / YDIM, 1.0f / ZDIM));
	marcher.SetUniform1i("volume", 1);
	marcher.Unbind();
	/* Marcher to be cut from program for now */

	Shader particle("res/shaders/particle.vert", "res/shaders/particle.frag");

	/* Shader to test if noise texture is working by rendering to fill screen quad */
	//Shader shader("res/shaders/vertex.shader", "res/shaders/fragment.shader");
	//shader.Bind();
	//shader.SetUniform1i("noise", 0);

	glGenVertexArrays(1, &cubeVAOID);
	glGenBuffers(1, &cubeVBOID);
	glGenBuffers(1, &cubeIndicesID);

	//unit cube vertices 
	glm::vec3 vertices[8] = { glm::vec3(-0.5f,-0.5f,-0.5f),
							glm::vec3(0.5f,-0.5f,-0.5f),
							glm::vec3(0.5f, 0.5f,-0.5f),
							glm::vec3(-0.5f, 0.5f,-0.5f),
							glm::vec3(-0.5f,-0.5f, 0.5f),
							glm::vec3(0.5f,-0.5f, 0.5f),
							glm::vec3(0.5f, 0.5f, 0.5f),
							glm::vec3(-0.5f, 0.5f, 0.5f) };

	//unit cube indices
	GLushort cubeIndices[36] = { 0,5,4,
							  5,0,1,
							  3,7,6,
							  3,6,2,
							  7,4,6,
							  6,4,5,
							  2,1,3,
							  3,1,0,
							  3,0,7,
							  7,0,4,
							  6,5,2,
							  2,5,1 };

	glBindVertexArray(cubeVAOID);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBOID);
	//pass cube vertices to buffer object memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0].x), GL_STATIC_DRAW);
	//enable vertex attributre array for position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//pass indices to element array  buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	//enable depth test
	glEnable(GL_DEPTH_TEST);

	//set the over blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!window.windowShouldClose())
	{
		window.start();

		/* Setup particle shader uniforms and call it */
		update.Bind();
		update.SetUniformVec3("dim", glm::vec3(XDIM, YDIM, ZDIM));
		update.SetUniform1f("time", glfwGetTime());
		update.SetUniform1f("delta_time", window.getDeltaTime());
		//update.SetUniform1f("delta_time", 0.7f);
		update.SetUniform1f("turn_speed", turn_speed);
		update.SetUniform1i("sample_num", sample_num);
		update.SetUniform1f("sensor_dist", sensor_dist);
		update.SetUniform1f("move_dist", move_dist);

		/* Set sampler to noise texture */
		//update.SetUniform1i("noise", 0);
		glDispatchCompute(PNUM, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//Camera position and Model View Projection Matrix
		glm::vec3 camPos = window.getCamPosition();
		glm::mat4 MVP = window.calcMVP();

		//clear colour and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBindVertexArray(cubeVAOID);
		//bind the raycasting shader
		marcher.Bind();
		marcher.SetUniformMat4("MVP", MVP);
		marcher.SetUniformVec3("camPos", camPos);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		marcher.Unbind();
		glDisable(GL_BLEND);

		//particle.Bind();
		//particle.SetUniformMat4("MVP", MVP);
		//particle.SetUniformVec3("dim", glm::vec3(XDIM, YDIM, ZDIM));
		//glDrawArrays(GL_POINTS, 0, PNUM);

		/* Test full screen quad */
		//shader.Bind();
		//glBindVertexArray(vao);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/* Decay compute shader */
		blur.Bind();
		blur.SetUniform1f("decay_rate", decay);
		blur.SetUniform1f("blur_factor", blur_factor);
		blur.SetUniform1f("delta_time", window.getDeltaTime());
		//blur.SetUniform1f("delta_time", 1.0f);
		glDispatchCompute(XDIM, YDIM, ZDIM);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		/* Copy compute shader */
		copy.Bind();
		glDispatchCompute(XDIM, YDIM, ZDIM);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		window.end();
	}

	glfwTerminate();
	return 0;
}