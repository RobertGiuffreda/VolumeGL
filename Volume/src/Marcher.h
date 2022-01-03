#pragma once
#include <glad/glad.h>
#include <glm.hpp>
#include "Shader.h"

class Marcher {
public:
	Marcher(int slot, glm::vec3 step);
	~Marcher();
	void render(glm::mat4 MVP, glm::vec3 cam_pos);
private:
	Shader shader;
	unsigned int vao_id;
	unsigned int vbo_id;
	unsigned int ibo_id;
};