#include<iostream>
#include<glad//glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc//matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<cstdlib>
#include<cmath>

// Подключение кастомных абстракций (RAII/Wrapper-классов над объектами OpenGL)
#include"shaderClass.h"
#include"Camera.h"
#include"GameObject.h"
#include"Cube.h"


int main()
{
	system("chcp 1251 > nul"); // Локализация вывода консоли (кодовая страница Windows-1251)

	glfwInit(); // Инициализация подсистемы GLFW

	// Конфигурация контекста дескриптора окна (OpenGL 3.3 Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Отсечение deprecated-функционала

	int WIDTH = 1920;
	int HEIGHT = 1080;

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

	glEnable(GL_DEPTH_TEST);

	//Face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

	double lastTime = glfwGetTime();
	double lastTimeFPS = glfwGetTime();
	int nbFrames = 0;
	GLuint timeID = glGetUniformLocation(shaderProgram.ID, "time");
	
	GameObject cubik(Cube::vertices, sizeof(Cube::vertices), Cube::indices, sizeof(Cube::indices), "wall.jpg", shaderProgram);
	cubik.transform.rotation = glm::vec3(45.0f, 45.0f, 45.0f);

	GameObject plane(Cube::vertices, sizeof(Cube::vertices), Cube::indices, sizeof(Cube::indices), "floor.jpg", shaderProgram);
	plane.transform.scale.y = 0.1f;
	plane.transform.scale.x = 10.0f;
	plane.transform.scale.z = 10.0f;
	plane.transform.position.y = -3.0f;

	GLuint offsetID = glGetUniformLocation(shaderProgram.ID, "offset");

	// Основной цикл обработки сообщений и рендеринга (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		// Заполнение буфера кадра заданным цветом (Clear Color Stage)
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Инвалидация и очистка цветового буфера

		shaderProgram.Activate(); // Инжект шейдерной программы в текущий пайплайн

		//Дельта тайм и FPS
		double curTime = glfwGetTime();
		float deltaTime = (float)(curTime - lastTime);
		lastTime = curTime;

		glUniform1f(timeID, curTime);

		nbFrames++;

		if (curTime - lastTimeFPS >= 1.0)
		{
			double fps = double(nbFrames);

			std::string windowTitle = "FPS: " + std::to_string(int(fps));

			glfwSetWindowTitle(window, windowTitle.c_str());

			nbFrames = 0;
			lastTimeFPS += 1.0;
		}

		//Камера
		camera.Inputs(window, deltaTime);
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		glUniform2f(offsetID, 0.8f, 0.8f);
		cubik.DrawObject(shaderProgram);

		glUniform2f(offsetID, 3.0f, 3.0f);
		plane.DrawObject(shaderProgram);

		glfwSwapBuffers(window); // Смена переднего и заднего буферов (Double Buffering)

		glfwPollEvents(); // Опрос системной очереди событий (ввод, изменение геометрии окна)
	}

	cubik.DeleteObject();
	plane.DeleteObject();
	shaderProgram.Delete();

	glfwDestroyWindow(window); // Уничтожение дескриптора окна
	glfwTerminate();           // Корректное завершение работы подсистемы GLFW
	return 0;
}