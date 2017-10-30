#version 440 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 color;

in vec3 FragPosition;
in vec3 Normal;

uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main()
{
	// ambient lighting
	vec3 ambient = light.ambient * material.ambient;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light.position - FragPosition);
	float diff = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// specular
	vec3 viewDirection = normalize(viewPosition - FragPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0f);
}