#pragma once

#include <glm.hpp>
#include <glad/glad.h>
#include "Shader.h"

class FullScreenTexture {
public:
	FullScreenTexture(int slot);
	~FullScreenTexture();
	void render();
private:
	Shader shader;
	unsigned int vao_id;
	unsigned int vbo_id;
	unsigned int ibo_id;
};