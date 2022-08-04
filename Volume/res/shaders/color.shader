#version 430 core

struct settings
{
	vec4 color;
};

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(rgba32f, binding = 1) uniform image3D trail_map;
layout(rgba32f, binding = 3) uniform image3D color_map;

layout(std430, binding = 1) buffer settings_map
{
	settings s[];
};

uniform int group_num;

void main()
{
	ivec3 px = ivec3(gl_GlobalInvocationID);
	vec4 map = imageLoad(trail_map, px);

	vec4 color = vec4(0.0f);
	for (int i = 0; i < group_num; ++i) {
		vec4 mask = vec4(i == 0, i == 1, i == 2, i == 3);
		color += s[i].color * dot(mask, map);
	}
	imageStore(color_map, px, color);
}