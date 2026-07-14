#version 330 core // Указывает версию OpenGL (3.3) и профиль Core (современный, без устаревших функций)

// Входные данные (атрибуты вершин), которые передаются из C++ кода через VAO/VBO
layout (location = 0) in vec3 aPos;   // Координаты вершины (X, Y, Z) на позиции 0
layout (location = 1) in vec3 aColor; // Цвет вершины (R, G, B) на позиции 1
layout (location = 2) in vec2 aTex;

out vec3 color; // Выходной параметр: передает интерполированный цвет в следующий (фрагментный) шейдер

out vec2 texCoord;

uniform float scale; // Глобальная переменная (юниформ) из C++ для динамического изменения масштаба

void main()
{
	// Вычисляем итоговые координаты вершины на экране с учетом масштабирования (scale).
	// Формула (aPos + aPos * scale) увеличивает или уменьшает фигуру относительно центра координат.
	gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);
	
	color = aColor; // Передаем входной цвет во фрагментный шейдер без изменений
	texCoord = aTex;
}
