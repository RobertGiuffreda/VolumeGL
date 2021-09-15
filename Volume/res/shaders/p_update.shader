#version 430 core

struct particle
{
	vec4 pos;
	vec4 dir;
	vec4 col;
};

layout(local_size_x = 16, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer particles
{
	particle p[];
};
layout(rgba32f, binding = 1) uniform image3D trail_map;

uniform vec3 dim;			// Dimensions of texture
uniform float time;			// Current glfw time
uniform float delta_time;
uniform int sample_num;
uniform float sensor_dist;
uniform float turn_speed;
uniform float move_dist;
uniform float repulsion;

/* Gaussian noise texture Two channels */
uniform sampler2D noise;

// Hash functions from https://www.shadertoy.com/view/4djSRW
vec3 hash31(float p)
{
	vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.xxy + p3.yzz) * p3.zyx);
}

vec2 hash21(float p)
{
	vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.xx + p3.yz) * p3.zy);
}

vec2 hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.xx + p3.yz) * p3.zy);

}

// Hash function from H. Schechter & R. Bridson, goo.gl/RXiKaH
uint hash(uint s) {
	s ^= 2747636419u;
	s *= 2654435769u;
	s ^= s >> 16;
	s *= 2654435769u;
	s ^= s >> 16;
	s *= 2654435769u;
	return s;
}

float Random(uint seed)
{
	return float(hash(seed)) / 4294967295.0f;
}


void main()
{
	vec3 constants = vec3(0.0f, 1.0f, 0.5f);
	uint gid = gl_GlobalInvocationID.x;

	float rr = p[gid].pos.x * gid + p[gid].pos.y * time + p[gid].pos.z * dim.x + 1.0f;
	uint ru = uint(rr);

	/* Algorithm used to choose a direction to turn towards relies on
	a uniform distribution. We dont have that currently. Really should
	get that bit figured out */

	/* Sense using the texture and decide direction change from that */
	// Set up coordinate system local to particle.
	vec3 w = p[gid].dir.xyz;					// Local forward
	vec3 wup = (abs(w.x) > 0.1f) ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 u = normalize(cross(wup, w));			// Local right
	vec3 v = cross(w, u);						// Local up
	mat3 ltw = mat3(u, v, w);					// [u, v, w]

	/* Sample many points ahead of particle and take the direction
	with the largest value in the loop below. */
	vec3 max_dir = constants.xxy;
	float max_val = imageLoad(trail_map, ivec3(p[gid].pos.xyz + sensor_dist * w)).w;
	int i; vec3 rand;
	for (i = 0; i < sample_num; i++)
	{
		rand = vec3(normalize(vec2(Random(ru + i), Random(hash(ru + i))) - 0.5f), 0.5f);
		vec3 world_rand = ltw * normalize(rand);	// Move to world coords from local coords

		/* Get the coord for the pixel by adding the random
		direction converted to local coordinates to the position
		of the particle.
		Then use the alpha channel value in the texture at the coords
		to test if the new direction should be chosen with max */
		ivec3 tmp_px = ivec3(p[gid].pos.xyz + sensor_dist * world_rand);
		float tmp_val = imageLoad(trail_map, tmp_px).w;
		if (tmp_val > max_val)
		{
			max_val = tmp_val;
			max_dir = rand;
		}
	}
	if (max_dir != constants.xxy)
	{
		/* Now we introduce randomness to the movement of the particle in a
		fashion similar to a version of the 2D Phyrasum algorithm. Construct
		a plane from the w vector and the chosen direction. Imagine the plane
		is split in twain by the w vector, we want a random direction on the
		side of the plane the chosen direction lies. We will do this in local
		coords and move to world with ltw.
		We start by taking the projection of max_dir onto the xy plane to get
		a unit vector on the plane that is orthogonal to the known vector, w
		in world coords and z in local coords.
		Then we take a random point on the unit circle on the plane.*/
	
		/* Because we are in local coords and xy is the plane we wish to
		project onto all we have to do is subtract the z coord. */
		vec3 s = normalize(vec3(max_dir.xy, 0.0f));
	
		/* Take random values to use as z and s weights */
		vec2 vals = hash21(rr + i) - 0.5f;
		//vec2 vals = texture(noise, hash21(rr + i)).xy;
	
		/* Move back to world coords*/
		max_dir = (vec3(0.0f, 0.0f, vals.x) + abs(vals.y) * s);
	}

	p[gid].dir.xyz = normalize(mix(p[gid].dir.xyz, ltw * normalize(max_dir), turn_speed * delta_time));
	vec3 n_pos = p[gid].pos.xyz + p[gid].dir.xyz * move_dist * delta_time;

	/* The position of the bounds in world space need
	to be between zero and dim values to line up with
	the 3D Texture coords. Won't have box at origin. */
	if (n_pos.x < 0.0f || n_pos.x >= dim.x || n_pos.y < 0.0f || n_pos.y >= dim.y || n_pos.z < 0.0f || n_pos.z >= dim.z)
	{
		n_pos.x = min(dim.x - 0.01f, max(n_pos.x, 0.0f));
		n_pos.y = min(dim.y - 0.01f, max(n_pos.y, 0.0f));
		n_pos.z = min(dim.z - 0.01f, max(n_pos.z, 0.0f));
		p[gid].dir.xyz = normalize(hash31(rr) - 0.5);
	}
	p[gid].pos.xyz = n_pos;

	ivec3 px = ivec3(p[gid].pos.xyz);
	imageStore(trail_map, px, p[gid].col);
}