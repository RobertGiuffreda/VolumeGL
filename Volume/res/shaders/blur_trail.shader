#version 430 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(rgba32f, binding = 1) uniform image3D trail_map;
layout(rgba32f, binding = 2) uniform image3D blur_map;

uniform float decay;
uniform float blur;
uniform float delta_time;
uniform vec3 dim;

float eps = 0.00001f;

void main()
{
	ivec3 px = ivec3(gl_GlobalInvocationID);
	vec4 sum = vec4(0.0f);

	//for (int xo = -1; xo < 1; xo++) {
	//	for (int yo = -1; yo < 1; yo++) {
	//		for (int zo = -1; zo < 1; zo++) {
	//			ivec3 tmp_coord = px + ivec3(xo, yo, zo);
	//			sum += imageLoad(trail_map, tmp_coord) * blur;
	//		}
	//	}
	//}
	//
	//sum *= 1.0f / (1. + 26. * blur);

	/* 3D Gaussian Kernel */
	// sum += imageLoad(trail_map, ivec3(max(0, px.x - 1), max(0, px.y - 1), max(0, px.z - 1)));
	sum += imageLoad(trail_map, ivec3(max(0, px.x - 1), max(0, px.y - 1), px.z));
	// sum += imageLoad(trail_map, ivec3(max(0, px.x - 1), max(0, px.y - 1), min(dim.z - 1, px.z + 1)));
	sum += imageLoad(trail_map, ivec3(max(0, px.x - 1), px.y, max(0, px.z - 1)));
	sum += imageLoad(trail_map, ivec3(max(0, px.x - 1), px.y, px.z)) * 2.0f;
	sum += imageLoad(trail_map, ivec3(max(0, px.x - 1), px.y, min(dim.z - 1, px.z + 1)));
	//sum += imageLoad(trail_map, ivec3(max(0, px.x - 1), min(dim.y - 1, px.y + 1), max(0, px.z - 1)));	
	sum += imageLoad(trail_map, ivec3(max(0, px.x - 1), min(dim.y - 1, px.y + 1), px.z));
	//sum += imageLoad(trail_map, ivec3(max(0, px.x - 1), min(dim.y - 1, px.y + 1), min(dim.z - 1, px.z + 1)));

	sum += imageLoad(trail_map, ivec3(px.x, max(0, px.y - 1), max(0, px.z - 1)));
	sum += imageLoad(trail_map, ivec3(px.x, max(0, px.y - 1), px.z)) * 2.0f;
	sum += imageLoad(trail_map, ivec3(px.x, max(0, px.y - 1), min(dim.z - 1, px.z + 1)));
	sum += imageLoad(trail_map, ivec3(px.x, px.y, max(0, px.z - 1))) * 2.0f;
	sum += imageLoad(trail_map, ivec3(px.x, px.y, px.z)) * 4.0f;
	sum += imageLoad(trail_map, ivec3(px.x, px.y, min(dim.z - 1, px.z + 1))) * 2.0f;
	sum += imageLoad(trail_map, ivec3(px.x, min(dim.y - 1, px.y + 1), max(0, px.z - 1)));
	sum += imageLoad(trail_map, ivec3(px.x, min(dim.y - 1, px.y + 1), px.z)) * 2.0f;
	sum += imageLoad(trail_map, ivec3(px.x, min(dim.y - 1, px.y + 1), min(dim.z - 1, px.z + 1)));

	//sum += imageLoad(trail_map, ivec3(min(dim.x - 1, px.x + 1), max(0, px.y - 1), max(0, px.z - 1)));
	sum += imageLoad(trail_map, ivec3(min(dim.x - 1, px.x + 1), max(0, px.y - 1), px.z));
	//sum += imageLoad(trail_map, ivec3(min(dim.x - 1, px.x + 1), max(0, px.y - 1), min(dim.z - 1, px.z + 1)));
	sum += imageLoad(trail_map, ivec3(min(dim.x - 1, px.x + 1), px.y, max(0, px.z - 1)));
	sum += imageLoad(trail_map, ivec3(min(dim.x - 1, px.x + 1), px.y, px.z)) * 2.0f;
	sum += imageLoad(trail_map, ivec3(min(dim.x - 1, px.x + 1), px.y, min(dim.z - 1, px.z + 1)));
	//sum += imageLoad(trail_map, ivec3(min(dim.x - 1, px.x + 1), min(dim.y - 1, px.y + 1), max(0, px.z - 1)));
	sum += imageLoad(trail_map, ivec3(min(dim.x - 1, px.x + 1), min(dim.y - 1, px.y + 1), px.z));
	//sum += imageLoad(trail_map, ivec3(min(dim.x - 1, px.x + 1), min(dim.y - 1, px.y + 1), min(dim.z - 1, px.z + 1)));

	sum /= 28.0f;

	sum = mix(imageLoad(trail_map, px), sum, blur * delta_time);
	sum = mix(sum, sum * decay, delta_time);
	if (sum.w < eps) sum = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	imageStore(blur_map, px, sum);
}
