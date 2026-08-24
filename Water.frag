#version 330 core
out vec4 FragColor;

in vec3 color;      // Исходный интерполированный цвет вершин (R, G, B)

in vec2 texCoord;

in float YPOS;

uniform float time; // Время из C++
uniform sampler2D tex0;

vec3 localColor = color;

void main()
{
	float gCol = 76 + sin(-YPOS) * 60;
	float bCol = 150 + sin(-YPOS) * 77;

	vec3 newColor = vec3(0.0,gCol/255,bCol/255);

	FragColor = vec4(newColor * 1.2, 1.0);
}
