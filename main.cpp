#include<iostream>
#include<glad//glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>

// Подключение кастомных абстракций (RAII/Wrapper-классов над объектами OpenGL)
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Texture.h"

// Интерливированный (interleaved) массив вершинных атрибутов: Позиция (vec3) и Цвет (vec3)
GLfloat vertices[] = {
	// Смещение: 0 (X, Y, Z) // Смещение: 12 байт (R, G, B)
	-1.0f, -1.0f, 0.0f,    1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Вершина 0
	 1.0f, -1.0f, 0.0f,    0.0f, 1.0f, 0.0f,	1.0f, 0.0f, // Вершина 1
	 1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Вершина 2
	-1.0f,  1.0f, 0.0f,    1.0f, 1.0f, 1.0f,	0.0f, 1.0f  // Вершина 3
};

// Индексный массив (Element Array) для явного указания топологии примитивов
GLuint indices[] = {
	0, 3, 2,
	2, 1, 0
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

	GLfloat backgroundColor[] = { 0.69f, 0.55f, 0.8f }; // Нормализованные RGBA значения цвета очистки
	GLfloat scale = -0.5f;                               // Скалярное значение для uniform-переменной

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

	// Получение хэндлов (расположений) uniform-переменных из скомпилированной шейдерной программы
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");
	GLuint timeID = glGetUniformLocation(shaderProgram.ID, "time"); // ДОБАВИЛИ: Локация uniform-переменной времени

	//Текстура
	Texture grassBlock("grass.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	grassBlock.texIUnit(shaderProgram, "tex0", 0);

	// Основной цикл обработки сообщений и рендеринга (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		// Заполнение буфера кадра заданным цветом (Clear Color Stage)
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT); // Инвалидация и очистка цветового буфера

		shaderProgram.Activate(); // Инжект шейдерной программы в текущий пайплайн

		// Динамическое обновление стейта на основе встроенного высокоточного таймера GLFW
		float currentTime = glfwGetTime(); // Получение дельты времени в секундах

		glUniform1f(uniID, scale); // Передача скаляра масштабирования в uniform-регистр GPU
		glUniform1f(timeID, currentTime); // ДОБАВИЛИ: Передача значения времени для внутренней процедурной графики фрагментного шейдера
		grassBlock.Bind();

		VAO1.Bind(); // Контекстная активация сконфигурированных вершинных атрибутов

		// Элементный (индексный) рендеринг примитивов по данным из EBO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
