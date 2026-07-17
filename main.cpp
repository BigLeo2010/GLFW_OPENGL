#include<iostream>
#include<glad//glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc//matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

// Подключение кастомных абстракций (RAII/Wrapper-классов над объектами OpenGL)
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Texture.h"
#include"Camera.h"

// Интерливированный (interleaved) массив вершинных атрибутов: Позиция (vec3), Цвет (vec3) и Текстура (vec2)
GLfloat vertices[] = {
	// Позиция (X, Y, Z)     // Цвет (R, G, B)        // Текстура (U, V)

	// --- ПЕРЕДНЯЯ ГРАНЬ
	-0.5f, -0.5f,  0.5f,     1.0f, 1.0f, 0.0f,        0.5f, 0.5f, // 0
	 0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,        1.0f, 0.5f, // 1
	 0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        1.0f, 1.0f, // 2
	-0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 1.0f,        0.5f, 1.0f, // 3

	// --- ЗАДНЯЯ ГРАНЬ
	-0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        1.0f, 0.5f, // 4
	 0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,        0.5f, 0.5f, // 5
	 0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,        0.5f, 1.0f, // 6
	-0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 1.0f,        1.0f, 1.0f, // 7

	// --- ЛЕВАЯ ГРАНЬ
	-0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        0.5f, 0.5f, // 8
	-0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,        1.0f, 0.5f, // 9
	-0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        1.0f, 1.0f, // 10
	-0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 1.0f,        0.5f, 1.0f, // 11

	// --- ПРАВАЯ ГРАНЬ
	 0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        1.0f, 0.5f, // 12
	 0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,        0.5f, 0.5f, // 13
	 0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        0.5f, 1.0f, // 14
	 0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 1.0f,        1.0f, 1.0f, // 15

	 // --- НИЖНЯЯ ГРАНЬ
	 -0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        0.0f, 0.0f, // 16
	  0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,        0.5f, 0.0f, // 17
	  0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        0.5f, 0.5f, // 18
	 -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 1.0f,        0.0f, 0.5f, // 19

	 // --- ВЕРХНЯЯ ГРАНЬ
	 -0.5f,  0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        0.0f, 0.5f, // 20
	  0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,        0.5f, 0.5f, // 21
	  0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        0.5f, 1.0f, // 22
	 -0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 1.0f,        0.0f, 1.0f  // 23
};


// Индексный массив (Element Array) для явного указания топологии примитивов
GLuint indices[] = {
	0,  1,  2,     0,  2,  3,  // Передняя
	4,  5,  6,     4,  6,  7,  // Задняя
	8,  9,  10,    8,  10, 11, // Левая
	12, 13, 14,    12, 14, 15, // Правая
	16, 17, 18,    16, 18, 19, // Нижняя
	20, 21, 22,    20, 22, 23  // Верхняя
};



int main()
{
	system("chcp 1251 > nul"); // Локализация вывода консоли (кодовая страница Windows-1251)

	glfwInit(); // Инициализация подсистемы GLFW

	// Конфигурация контекста дескриптора окна (OpenGL 3.3 Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Отсечение deprecated-функционала

	int WIDTH = 800;
	int HEIGHT = 800;

	GLfloat backgroundColor[] = { 70.0f/255.0f, 126.0f/255.0f, 199.0f/255.0f }; // Нормализованные RGBA значения цвета очистки

	// Инстанцирование объекта окна и создание ассоциированного контекста OpenGL
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello world!", NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create a window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Привязка контекста OpenGL к текущему потоку выполнения

	gladLoadGL(); // Динамическая загрузка указателей на функции API OpenGL через GLAD

	glViewport(0, 0, WIDTH, HEIGHT); // Задание матрицы преобразования координат в пиксели окна (NDC to Window Space)

	// Инициализация графического конвейера (компиляция и линковка шейдеров)
	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1;     // Генерация Vertex Array Object (хранилище состояний вершинных атрибутов)
	VAO1.Bind();  // Активация VAO для записи последующих конфигураций буферов

	// Выделение VRAM и аллокация данных (копирование массивов в видеопамять)
	VBO VBO1(vertices, sizeof(vertices)); // Аллокация GL_ARRAY_BUFFER
	EBO EBO1(indices, sizeof(indices));   // Аллокация GL_ELEMENT_ARRAY_BUFFER (автоматически привязывается к текущему VAO)

	// Описание макета данных (Layout) для вершинных атрибутов внутри VAO:
	// Атрибут 0 (Координаты): компонентность 3 (vec3), тип float, шаг 24 байта, смещение 0
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	// Атрибут 1 (Цвет): компонентность 3 (vec3), тип float, шаг 24 байта, смещение 12 байт
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// Атрибут 2 (Текстура): компонентность 2 (vec2), тип float, шаг 24 байта, смещение 12 байт
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	VAO1.Unbind(); // Сброс состояния VAO (защита от непреднамеренной мутации стейта)
	VBO1.Unbind(); // Развязка GL_ARRAY_BUFFER
	EBO1.Unbind(); // Развязка GL_ELEMENT_ARRAY_BUFFER

	//Текстура
	Texture grassBlock("grass_text.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	grassBlock.texIUnit(shaderProgram, "tex0", 0);

	glEnable(GL_DEPTH_TEST);

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

	double lastTime = glfwGetTime();

	// Основной цикл обработки сообщений и рендеринга (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		// Заполнение буфера кадра заданным цветом (Clear Color Stage)
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Инвалидация и очистка цветового буфера

		shaderProgram.Activate(); // Инжект шейдерной программы в текущий пайплайн

		//Дельта тайм
		double curTime = glfwGetTime();
		float deltaTime = (float)(curTime - lastTime);
		lastTime = curTime;

		//Камера
		camera.Inputs(window, deltaTime);
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		VAO1.Bind(); // Контекстная активация сконфигурированных вершинных атрибутов

		grassBlock.Bind();
		// Элементный (индексный) рендеринг примитивов по данным из EBO
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window); // Смена переднего и заднего буферов (Double Buffering)

		glfwPollEvents(); // Опрос системной очереди событий (ввод, изменение геометрии окна)
	}

	// Освобождение ресурсов GPU (Деаллокация объектов в видеопамяти)
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	grassBlock.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window); // Уничтожение дескриптора окна
	glfwTerminate();           // Корректное завершение работы подсистемы GLFW
	return 0;
}