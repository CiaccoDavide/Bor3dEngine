#version 440 core

layout(location = 0) in vec3 position;

// for projection
uniform mat4 model;			// this matrix converts local objects coords into camera coords
uniform mat4 view;			// normalized coords to window coords
uniform mat4 projection;	// converts camera coords to normalized coords [0,1]

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
}