#version 440 core

out vec4 color;

in vec3 FragPosition;
in vec3 Normal;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	// ambient lighting
	float ambientStrength = 0.5f;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPosition - FragPosition);
	float diff = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 1.4f;
	vec3 viewDirection = normalize(viewPosition - FragPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	color = vec4(result, 1.0f);
}