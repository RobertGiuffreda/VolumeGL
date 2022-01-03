#include "Particles.h"

Particles::Particles(glm::vec3 dim, size_t p_num, Layout distribution, Direction direction) : p_num(p_num)
{
	glm::vec4 center = glm::vec4(dim, 0.0f) * 0.5f;
	particles.resize(p_num);
	for (int i = 0; i < p_num; i++)
	{
		glm::vec3 pos;
		/* Particle Position */
		switch (distribution)
		{
		case Layout::CENTER:
			particles[i].position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) + center;
			break;
		case Layout::SHELL:
			pos = glm::normalize(glm::vec3(standardNormal(), standardNormal(), standardNormal()));
			particles[i].position = glm::vec4(pos, 1.0f) + center;
			break;
		case Layout::SPHERE:
			pos = glm::normalize(glm::vec3(standardNormal(), standardNormal(), standardNormal()));
			pos *= glm::pow(uniform(), (1.0f / 3.0f)) * (dim.x / 2);
			particles[i].position = glm::vec4(pos, 1.0f) + center;
			break;
		case Layout::CUBE:
			pos = glm::vec3(uniform(), uniform(), uniform()) * dim;
			particles[i].position = glm::vec4(pos, 1.0f);
			break;
		}

		/* Particle Direction */
		switch (direction)
		{
		case Direction::IN:
			particles[i].direction = center - particles[i].position;
			break;
		case Direction::OUT:
			particles[i].direction = particles[i].position - center;
			break;
		case Direction::RANDOM:
			particles[i].direction = glm::vec4(glm::normalize(glm::vec3(standardNormal(), standardNormal(), standardNormal())), 1.0f);
			break;
		}

		/* Particle Color */
		particles[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

std::vector<particle> Particles::get_particles()
{
	return particles;
}