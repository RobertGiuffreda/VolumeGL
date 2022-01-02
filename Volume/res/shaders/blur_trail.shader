#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 1) uniform image3D trail_map;
layout(rgba32f, binding = 2) uniform image3D blur_map;

uniform float decay_rate;
uniform float blur_factor;
uniform float delta_time;

float eps = 0.01f;

void main()
{
	ivec3 px_coord = ivec3(gl_GlobalInvocationID);
	vec4 sum = vec4(0.0f);
	for (int xo = -1; xo < 1; xo++) {
		for (int yo = -1; yo < 1; yo++) {
			for (int zo = -1; zo < 1; zo++) {
				ivec3 tmp_coord = ivec3(px_coord.x + xo, px_coord.y + yo, px_coord.z + zo);
				sum += imageLoad(trail_map, tmp_coord) * blur_factor;
			}
		}
	}
	sum *= 1.0f / (1. + 26. * blur_factor);

	sum = mix(imageLoad(trail_map, px_coord), sum, delta_time);
	sum = mix(sum, sum * decay_rate, delta_time);
	if (sum.w < eps) sum = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	imageStore(blur_map, px_coord, sum);
}
