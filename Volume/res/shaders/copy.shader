#version 430 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(rgba32f, binding = 1) uniform image3D trail_map;
layout(rgba32f, binding = 2) uniform image3D blur_map;

void main()
{
	ivec3 px_coord = ivec3(gl_GlobalInvocationID);
	imageStore(trail_map, px_coord, imageLoad(blur_map, px_coord));
}