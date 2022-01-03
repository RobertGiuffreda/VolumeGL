#include "FullScreenTexture.h"

FullScreenTexture::FullScreenTexture(int slot) : shader("res/shaders/vertex.shader", "res/shaders/fragment.shader")
{
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

	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);

	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * 4, screen_poly, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, screen_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	shader.Bind();
	shader.SetUniform1i("noise", slot);
	shader.Unbind();
}

FullScreenTexture::~FullScreenTexture()
{
	glDeleteVertexArrays(1, &vao_id);
	glDeleteBuffers(1, &vbo_id);
	glDeleteBuffers(1, &ibo_id);
}

void FullScreenTexture::render()
{
	shader.Bind();
	glBindVertexArray(vao_id);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	shader.Unbind();
}