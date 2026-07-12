#include<iostream>
#include<glad//glad.h>
#include<GLFW/glfw3.h>

// Подключаем новые ООП-классы обертки
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

GLfloat vertices[] = {
	// Координаты (X, Y, Z)             // Цвета (R, G, B)
	-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,    1.0f, 0.0f, 0.0f,  // Точка 0: Яркий Красный
	 0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,    0.0f, 1.0f, 0.0f,  // Точка 1: Яркий Зеленый
	 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,   0.0f, 0.0f, 1.0f,  // Точка 2: Яркий Синий
};


// Индексы, указывающие OpenGL порядок сборки треугольников из точек выше
GLuint indices[] = {
	0, 1, 2
};


int main()
{
	system("chcp 1251 > nul"); //Исправляем кодировку в консоли

	glfwInit(); //Инициализируем GLFW

	//Настройка версии OpenGL (3.3 Core)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Мажорная версия 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Минорная версия 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Современный профиль

	//Создание окна
	int WIDTH = 800; //Ширина окна
	int HEIGHT = 800; //Высота окна

	GLfloat backgroundColor[] = { 0.69f, 0.55f, 0.8f };

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello world!", NULL, NULL);

	if (window == NULL) { //Если окно не создалось
		std::cout << "Failed to create a window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); //Делаем окно главным контекстом

	gladLoadGL(); //Инициализируем GLAD для работы с функциями OpenGL

	glViewport(0, 0, WIDTH, HEIGHT); //Задаем область отрисовки (от 0,0 до 800x600)

	// Создаем шейдерную программу, автоматически считывая файлы кода
	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1; // Создаем объект массива массивов (настроек)
	VAO1.Bind(); // Включаем его в контекст

	VBO VBO1(vertices, sizeof(vertices)); // Создаем буфер вершин и отправляем туда координаты
	EBO EBO1(indices, sizeof(indices)); // Создаем буфер индексов и отправляем туда порядок отрисовки

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Связываем VBO с текущим VAO (атрибут 0)
	VAO1.Unbind(); // Отключаем VAO, чтобы случайно не испортить настройки
	VBO1.Unbind(); // Отключаем VBO
	EBO1.Unbind(); // Отключаем EBO

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	//Главный цикл программы
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f); //Каждый кадр задаем зеленый цвет фона
		glClear(GL_COLOR_BUFFER_BIT); //Очищаем экран
		shaderProgram.Activate(); //Включаем наши шейдеры
		glUniform1f(uniID, 0.5f);
		VAO1.Bind(); //Включаем настройки нашей геометрической фигуры

		// Рисуем геометрию через индексный буфер
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window); //Выводим нарисованное на экран

		glfwPollEvents(); //Проверяем события (клики, кнопки)
	}

	// Освобождаем ресурсы перед выходом программы
	VAO1.Delete(); // Удаляем настройки VAO
	VBO1.Delete(); // Удаляем координаты VBO
	EBO1.Delete(); // Удаляем индексы EBO
	shaderProgram.Delete(); // Удаляем скомпилированные шейдеры

	glfwDestroyWindow(window); //Удаляем окно
	glfwTerminate(); //Очистка памяти
	return 0;
}
