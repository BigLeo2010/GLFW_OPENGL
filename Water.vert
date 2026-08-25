#version 330 core // Указывает версию OpenGL (3.3) и профиль Core (современный, без устаревших функций)

// Входные данные (атрибуты вершин), которые передаются из C++ кода через VAO/VBO
layout (location = 0) in vec3 aPos;   // Координаты вершины (X, Y, Z) на позиции 0
layout (location = 1) in vec3 aColor; // Цвет вершины (R, G, B) на позиции 1
layout (location = 2) in vec2 aTex;

out vec3 color; // Выходной параметр: передает интерполированный цвет в следующий (фрагментный) шейдер

out vec2 texCoord;

out float YPOS;

uniform float time;
uniform mat4 camMatrix;
uniform mat4 model;
uniform vec2 offset;

float amplitude = 0.15;
float frequency = 1.5;
float speed = 1.0;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);

	worldPos.y += amplitude * sin(frequency * worldPos.z + speed * time) * sin((frequency-0.5) * worldPos.x + (speed+0.2) * time);
	YPOS = worldPos.y;

	gl_Position = camMatrix * worldPos;
	
	color = aColor; // Передаем входной цвет во фрагментный шейдер без изменений
	texCoord = aTex * offset;
}
