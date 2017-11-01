#version 440 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPosition;
out vec2 TexCoords;

uniform mat4 model;			// this matrix converts local objects coords into camera coords
uniform mat4 view;			// normalized coords to window coords
uniform mat4 projection;	// converts camera coords to normalized coords [0,1]

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	FragPosition = vec3(model * vec4(position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = texCoords;
}
