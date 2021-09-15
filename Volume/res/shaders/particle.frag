#version 430 core

layout(location = 0) out vec4 fColor;

in vec4 vCol;

void main()
{
	fColor = vCol;
}