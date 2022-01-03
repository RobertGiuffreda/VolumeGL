#pragma once

#include <glad/glad.h>

class Texture {
public:
	unsigned int id;
	Texture(int w, int h, int l, int slot, int base);
	Texture(int w, int h, int slot, int base);
	~Texture();
};