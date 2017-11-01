#version 330 core

#define NUMBER_OF_POINT_LIGHTS 10

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutoff;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPosition;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPosition;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NUMBER_OF_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// Function prototypes
vec3 CalcDirectionalLight( DirectionalLight light, vec3 normal, vec3 viewDirection );
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 FragPosition, vec3 viewDirection );
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 FragPosition, vec3 viewDirection );

void main( )
{
    // Properties
    vec3 norm = normalize( Normal );
    vec3 viewDirection = normalize( viewPosition - FragPosition );
    
    // Directional lighting
    vec3 result = CalcDirectionalLight( directionalLight, norm, viewDirection );
    
    // Point lights
    for ( int i = 0; i < NUMBER_OF_POINT_LIGHTS; i++ )
    {
        result += CalcPointLight( pointLights[i], norm, FragPosition, viewDirection );
    }
    
    // Spot light
    result += CalcSpotLight( spotLight, norm, FragPosition, viewDirection );
    
    color = vec4( result, 1.0 );
}

// Calculates the color when using a directional light.
vec3 CalcDirectionalLight( DirectionalLight light, vec3 normal, vec3 viewDirection )
{
    vec3 lightDir = normalize( -light.direction );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDirection, reflectDir ), 0.0 ), material.shininess );
    
    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCoords ) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.diffuse, TexCoords ) );
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords ) );
    
    return ( ambient + diffuse + specular );
}

// Calculates the color when using a point light.
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 FragPosition, vec3 viewDirection )
{
    vec3 lightDir = normalize( light.position - FragPosition );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDirection, reflectDir ), 0.0 ), material.shininess );
    
    // Falloff
    float distance = length( light.position - FragPosition );
    float falloff = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCoords ) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.diffuse, TexCoords ) );
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords ) );
    
    ambient *= falloff;
    diffuse *= falloff;
    specular *= falloff;
    
    return ( ambient + diffuse + specular );
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 FragPosition, vec3 viewDirection )
{
    vec3 lightDir = normalize( light.position - FragPosition );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDirection, reflectDir ), 0.0 ), material.shininess );
    
    // Falloff
    float distance = length( light.position - FragPosition );
    float falloff = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Spotlight intensity
    float theta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp( ( theta - light.outerCutoff ) / epsilon, 0.0, 1.0 );
    
    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCoords ) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.diffuse, TexCoords ) );
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords ) );
    
    ambient *= falloff * intensity;
    diffuse *= falloff * intensity;
    specular *= falloff * intensity;
    
    return ( ambient + diffuse + specular );
}