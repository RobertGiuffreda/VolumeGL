#include "Marcher.h"

Marcher::Marcher(int slot, glm::vec3 step)
	: shader("res/shaders/marchervert.shader", "res/shaders/marcherfrag.shader"),
	vao_id(0), vbo_id(0), ibo_id(0)
{
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

	glGenVertexArrays(1, &vao_id);
	glGenBuffers(1, &vbo_id);
	glGenBuffers(1, &ibo_id);

	glBindVertexArray(vao_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_STATIC_DRAW);

	shader.Bind();
	shader.SetUniformVec3("step_size", step);
	shader.SetUniform1i("volume", slot);
	shader.Unbind();

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Marcher::~Marcher()
{
	glDeleteVertexArrays(1, &vao_id);
	glDeleteBuffers(1, &vbo_id);
	glDeleteBuffers(1, &ibo_id);
}

void Marcher::render(glm::mat4 MVP, glm::vec3 cam_pos)
{
	glEnable(GL_BLEND);
	glBindVertexArray(vao_id);
	shader.Bind();

	shader.SetUniformMat4("MVP", MVP);
	shader.SetUniformVec3("camPos", cam_pos);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);

	shader.Unbind();
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}