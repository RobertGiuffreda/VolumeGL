#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 1) uniform image3D trail_map;

void main()
{
	ivec3 px_coord = ivec3(gl_GlobalInvocationID);
	imageStore(trail_map, px_coord, vec4(0.0f, 0.0f, 0.0f, 0.0f));
}