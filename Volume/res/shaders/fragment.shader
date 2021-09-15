#version 430

out vec4 color;

in vec2 tex_coord;

uniform sampler2D noise;

void main()
{
	color = vec4(texture(noise, tex_coord).rrr, 1.0f);
}