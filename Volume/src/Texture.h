#pragma once

class Texture {
public:
	unsigned int id;
	Texture(int w, int h, int l, int slot, int base);
	~Texture() {}
};