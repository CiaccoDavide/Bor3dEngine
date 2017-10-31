#version 440 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 position;
	// vec3 direction; // directional light

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

in vec3 FragPosition;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main()
{
	// ambient lighting
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light.position - FragPosition);
	//vec3 lightDirection = normalize(-light.direction); // directional light
	float diff = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	// specular
	vec3 viewDirection = normalize(viewPosition - FragPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	// point light falloff
	float distance = length(light.position - FragPosition);
	float falloff = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	ambient *= falloff;
	diffuse *= falloff;
	specular *= falloff;

	color = vec4(ambient + diffuse + specular, 1.0f);
}