#version 430 core

struct particle {
	vec4 pos;
	vec4 dir;
	vec4 mask;
};

layout(std430, binding = 0) buffer particles
{
	particle p[];
};

out vec4 vCol;
uniform mat4 MVP;
uniform vec3 dim;

void main()
{
	vec3 pos = (p[gl_VertexID].pos.xyz - (dim * 0.5f))/dim;
	gl_Position = MVP * vec4(pos, 1.0f);
	vCol = vec4(p[gl_VertexID].mask.xyz, 1.0f);
}