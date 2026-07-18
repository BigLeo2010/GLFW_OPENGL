#include<iostream>
#include<glad//glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc//matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

// Подключение кастомных абстракций (RAII/Wrapper-классов над объектами OpenGL)
#include"shaderClass.h"
#include"Camera.h"
#include"GameObject.h"
#include"Cube.h"
#include"BlockRegistry.h"

void DrawArrayOfObjects(GameObject objects[], Shader& shader) {
	for (int i = 0; i < sizeof(objects)-1; ++i) {
		objects[i].DrawObject(shader);
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

	BlockRegistry::Initialize(shaderProgram); // Инициализация реестра блоков с передачей шейдера

	// Создаем куб, передаем данные геометрии, путь к картинке и шейдер
	GameObject grass = BlockRegistry::CreateBlock(BlockType::GRASS, glm::vec3(0.0f, 0.0f, 0.0f));

	GameObject wood = BlockRegistry::CreateBlock(BlockType::WOOD, glm::vec3(1.5f, 0.0f, 0.0f));

	GameObject dirt = BlockRegistry::CreateBlock(BlockType::DIRT, glm::vec3(-1.5f, 0.0f, 0.0f));

	GameObject sand = BlockRegistry::CreateBlock(BlockType::SAND, glm::vec3(3.0f, 0.0f, 0.0f));

	GameObject stone = BlockRegistry::CreateBlock(BlockType::STONE, glm::vec3(-3.0f, 0.0f, 0.0f));

	GameObject iron_ore = BlockRegistry::CreateBlock(BlockType::IRON_ORE, glm::vec3(-4.5f, 0.0f, 0.0f));

	GameObject diamond_ore = BlockRegistry::CreateBlock(BlockType::DIAMOND_ORE, glm::vec3(-6.0f, 0.0f, 0.0f));

	GameObject cubes[7] = { grass, wood, dirt, sand, stone, iron_ore, diamond_ore };

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

		DrawArrayOfObjects(cubes, shaderProgram);

		glfwSwapBuffers(window); // Смена переднего и заднего буферов (Double Buffering)

		glfwPollEvents(); // Опрос системной очереди событий (ввод, изменение геометрии окна)
	}

	BlockRegistry::Clear(); // Очистка памяти реестра блоков
	shaderProgram.Delete();

	glfwDestroyWindow(window); // Уничтожение дескриптора окна
	glfwTerminate();           // Корректное завершение работы подсистемы GLFW
	return 0;
}