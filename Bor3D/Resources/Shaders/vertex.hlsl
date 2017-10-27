#version 440 core

layout(location = 0) in vec3 position;
//layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;

//out vec3 ourColor;
out vec2 TexCoord;

// for classic transformations
//uniform mat4 transform;

// for projection
uniform mat4 model;			// this matrix converts local objects coords into camera coords
uniform mat4 view;			// normalized coords to window coords
uniform mat4 projection;	// converts camera coords to normalized coords [0,1]

void main()
{
	// for classic transformations
	//gl_Position = transform * vec4(position, 1.0f);
	gl_Position = projection * view * model * vec4(position, 1.0f);
	//ourColor = color;
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}