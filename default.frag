#version 330 core
out vec4 FragColor;

in vec3 color;      // Исходный интерполированный цвет вершин (R, G, B)

in vec2 texCoord;

uniform float time; // Время из C++
uniform sampler2D tex0;

vec3 localColor = color;

void main()
{
	float rCol = localColor.x * sin(time * 0.8) * 0.5 + 0.5;
	float gCol = localColor.y * sin(time * 0.8) * 0.5 + 0.5;
	float bCol = localColor.z * sin(time * 0.8) * 0.5 + 0.5;

	vec3 newColor = vec3(rCol,gCol,bCol);

	FragColor = /*vec4(localColor, 1.0f);*/  texture(tex0, texCoord);
}
