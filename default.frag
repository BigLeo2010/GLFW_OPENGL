#version 330 core
out vec4 FragColor;

in vec2 texCoord;

in vec3 normal;
in vec3 curPos;

uniform sampler2D tex0;

vec3 lightColor = vec3(1.0,1.0,1.0);

uniform vec3 lightPos;
uniform vec3 viewPos;

float ambient = 0.2;
float specularStrength = 0.5;

void main()
{
	vec3 normal = normalize(normal);
	vec3 lightDirection = normalize(lightPos - curPos);

	vec3 viewDir = normalize(viewPos - curPos);
	vec3 reflectDir = reflect(-lightDirection, normal); 

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * specularStrength;

	float diffuse = max(dot(normal, lightDirection), 0.0);

	FragColor = texture(tex0, texCoord) * vec4(lightColor, 1.0) * (diffuse + ambient + spec);
}
