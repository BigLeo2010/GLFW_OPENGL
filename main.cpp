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


void generateProceduralPlane(int widthSegments, int heightSegments, float cellWidth, float cellHeight,
	std::vector<float>& outVertices, std::vector<unsigned int>& outIndices)
{
	int xCount = widthSegments + 1;
	int zCount = heightSegments + 1;

	outVertices.reserve(xCount * zCount * 8);
	for (int z = 0; z < zCount; ++z) {
		for (int x = 0; x < xCount; ++x) {
			// Позиция (X, Y, Z)
			outVertices.push_back(x * cellWidth);
			outVertices.push_back(0.0f);
			outVertices.push_back(z * cellHeight);

			// Цвет (R, G, B) — сделаем белым, чтобы текстура не искажалась
			outVertices.push_back(1.0f);
			outVertices.push_back(1.0f);
			outVertices.push_back(1.0f);

			// Текстурные координаты (U, V)
			outVertices.push_back(static_cast<float>(x) / widthSegments);
			outVertices.push_back(static_cast<float>(z) / heightSegments);
		}
	}

	outIndices.reserve(widthSegments * heightSegments * 6);
	for (int z = 0; z < heightSegments; ++z) {
		for (int x = 0; x < widthSegments; ++x) {
			unsigned int bottomLeft = x + z * xCount;
			unsigned int bottomRight = (x + 1) + z * xCount;
			unsigned int topLeft = x + (z + 1) * xCount;
			unsigned int topRight = (x + 1) + (z + 1) * xCount;

			// Обход вершин
			outIndices.push_back(bottomLeft);
			outIndices.push_back(bottomRight);
			outIndices.push_back(topLeft);

			outIndices.push_back(bottomRight);
			outIndices.push_back(topRight);
			outIndices.push_back(topLeft);
		}
	}
}


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

	GLfloat backgroundColor[] = { 45.0f/255.0f, 45.0f/255.0f, 45.0f/255.0f }; // Нормализованные RGBA значения цвета очистки

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
	Shader colorShader("colorShader.vert", "colorShader.frag");

	Shader waterShader("Water.vert", "Water.frag");

	glEnable(GL_DEPTH_TEST);

	//Face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

	double lastTime = glfwGetTime();
	double lastTimeFPS = glfwGetTime();
	int nbFrames = 0;
	
	GameObject cubik(Cube::vertices, sizeof(Cube::vertices), Cube::indices, sizeof(Cube::indices), "wall.jpg", shaderProgram);

	GameObject plane(Cube::vertices, sizeof(Cube::vertices), Cube::indices, sizeof(Cube::indices), "floor.jpg", shaderProgram);
	plane.transform.scale.y = 0.1f;
	plane.transform.scale.x = 10.0f;
	plane.transform.scale.z = 10.0f;
	plane.transform.position.y = -3.0f;

	GameObject lightCube(Cube::vertices, sizeof(Cube::vertices), Cube::indices, sizeof(Cube::indices), glm::vec3(1.0f,1.0f,1.0f), colorShader);
	glm::vec3 offset = glm::vec3(4.0f, 4.0f, 4.0f);;
	lightCube.transform.position = offset;
	lightCube.transform.scale *= 0.5f;

	std::vector<float> procVertices;
	std::vector<unsigned int> procIndices;

	generateProceduralPlane(40, 40, 0.2f, 0.2f, procVertices, procIndices);

	GameObject proceduralPlane(
		procVertices.data(),
		procVertices.size() * sizeof(float),
		procIndices.data(),
		procIndices.size() * sizeof(unsigned int),
		"floor.jpg",
		waterShader
	);

	proceduralPlane.transform.position.y = -3.0f;
	proceduralPlane.transform.position.x = 6.0f;
	proceduralPlane.transform.position.z = -4.0f;

	// Основной цикл обработки сообщений и рендеринга (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		// Заполнение буфера кадра заданным цветом (Clear Color Stage)
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Инвалидация и очистка цветового буфера

		//Дельта тайм и FPS
		double curTime = glfwGetTime();
		float deltaTime = (float)(curTime - lastTime);
		lastTime = curTime;

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
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

		//COLORS
		colorShader.Activate();

		camera.Matrix(colorShader, "camMatrix");

		lightCube.DrawObject(colorShader);

		lightCube.transform.position.x = 5 * sin(curTime);
		lightCube.transform.position.z = 5 * cos(curTime);

		//TEXTURES
		shaderProgram.Activate();
		shaderProgram.SetVec3("lightPos", lightCube.transform.position.x, lightCube.transform.position.y, lightCube.transform.position.z);
		shaderProgram.SetVec3("viewPos", camera.Position.x, camera.Position.z, camera.Position.z);

		camera.Matrix(shaderProgram, "camMatrix");

		shaderProgram.SetVec2("offset", 0.8f, 0.8f);
		cubik.DrawObject(shaderProgram);

		shaderProgram.SetVec2("offset", 3.0f, 3.0f);
		plane.DrawObject(shaderProgram);

		//CUSTOM
		waterShader.Activate();

		waterShader.SetFloat("time", (float)curTime);

		camera.Matrix(waterShader, "camMatrix");

		proceduralPlane.DrawObject(waterShader);


		glfwSwapBuffers(window); // Смена переднего и заднего буферов (Double Buffering)

		glfwPollEvents(); // Опрос системной очереди событий (ввод, изменение геометрии окна)
	}


	glfwDestroyWindow(window); // Уничтожение дескриптора окна
	glfwTerminate();           // Корректное завершение работы подсистемы GLFW
	return 0;
}