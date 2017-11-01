#version 440 core

in vec3 Color;
out vec4 color;

void main()
{
	color = vec4(vec3(2.6f)*Color, 0.5f);
}