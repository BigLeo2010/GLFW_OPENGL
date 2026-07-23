#define GLM_ENABLE_EXPERIMENTAL
#include<iostream>
#include<glad//glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include<glm/gtc//matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<cstdlib>

// Подключение кастомных абстракций (RAII/Wrapper-классов над объектами OpenGL)
#include"shaderClass.h"
#include"Camera.h"
#include"Cube.h"
#include"Texture.h"
#include"Chunk.h"
#include"Raycast.h"


int findClosestVectorIndex(const glm::vec3& x, const std::vector<glm::vec3>& list) {
	if (list.empty()) return -1; // Обработка пустого списка

	auto closest = std::min_element(list.begin(), list.end(),
		[&x](const glm::vec3& a, const glm::vec3& b) {
			return glm::distance2(x, a) < glm::distance2(x, b);
		}
	);

	return std::distance(list.begin(), closest);
}

int main()
{
	system("chcp 1251 > nul"); // Локализация вывода консоли (кодовая страница Windows-1251)

	srand(static_cast<unsigned int>(time(0)));

	glfwInit(); // Инициализация подсистемы GLFW

	// Конфигурация контекста дескриптора окна (OpenGL 3.3 Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Отсечение deprecated-функционала

	int WIDTH = 1920;
	int HEIGHT = 1080;

	GLfloat backgroundColor[] = { 70.0f/255.0f, 126.0f/255.0f, 199.0f/255.0f }; // Нормализованные RGBA значения цвета очистки

	// Инстанцирование объекта окна и создание ассоциированного контекста OpenGL
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Smoke'nCraft", NULL, NULL);

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
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 40.0f, 2.0f));

	double lastTime = glfwGetTime();

	Texture chunkText("textures.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	chunkText.texIUnit(shaderProgram, "tex0", 0);

	std::vector<Chunk> world;
	world.reserve(1000);

	for (int x = 0; x < 20; x++)
	{
		for (int z = 0; z < 20; z++)
		{
			world.emplace_back(x, 0, z);

			world.back().FillChunkData();
			world.back().BuildChunkMesh();
			world.back().UploadChunkToGPU();
		}
	}

	bool leftMousePressedLastFrame = false;
	bool rightMousePressedLastFrame = false;

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
		camera.Matrix(45.0f, 0.1f, 2000.0f, shaderProgram, "camMatrix");

		bool isRightClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

		if (isRightClicked && !rightMousePressedLastFrame)
		{
			// Запускаем трассировку луча длиной 5.5 блоков
			Raycast::HitResult hitResult = Raycast::Trace(camera, world, 10.0f);

			// Если во что-то попали и целевой чанк существует
			if (hitResult.hit && hitResult.placeChunk != nullptr)
			{
				// Переводим мировую позицию установки в локальные координаты чанка
				int cx = hitResult.placeChunk->GetX();
				int cz = hitResult.placeChunk->GetZ();

				int localX = hitResult.placePos.x - (cx * Chunk::CHUNK_SIZE_X);
				int localY = hitResult.placePos.y;
				int localZ = hitResult.placePos.z - (cz * Chunk::CHUNK_SIZE_Z);

				// Ставим блок Травы (ID = 1)
				hitResult.placeChunk->SetBlock(localX, localY, localZ, 3);

				// Перегенерация меша и обновление данных на GPU
				hitResult.placeChunk->BuildChunkMesh();
				hitResult.placeChunk->UploadChunkToGPU(); // Убедись, что метод в твоем main называется именно так
			}
		}
		rightMousePressedLastFrame = isRightClicked;


		bool isLeftClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

		if (isLeftClicked && !leftMousePressedLastFrame)
		{
			Raycast::HitResult hitResult = Raycast::Trace(camera, world, 10.0f);

			// Если во что-то попали и целевой чанк существует
			if (hitResult.hit && hitResult.targetChunk != nullptr)
			{
				// Переводим мировую позицию установки в локальные координаты чанка
				int cx = hitResult.targetChunk->GetX();
				int cz = hitResult.targetChunk->GetZ();

				int localX = hitResult.blockPos.x - (cx * Chunk::CHUNK_SIZE_X);
				int localY = hitResult.blockPos.y;
				int localZ = hitResult.blockPos.z - (cz * Chunk::CHUNK_SIZE_Z);

				// Удаляем блок
				hitResult.targetChunk->SetBlock(localX, localY, localZ, 0);

				// Перегенерация меша и обновление данных на GPU
				hitResult.targetChunk->BuildChunkMesh();
				hitResult.targetChunk->UploadChunkToGPU(); // Убедись, что метод в твоем main называется именно так
			}
		}
		leftMousePressedLastFrame = isLeftClicked;


		chunkText.Bind();

		for (auto& chunk : world)
		{
			chunk.DrawChunk(shaderProgram);
		}

		glfwSwapBuffers(window); // Смена переднего и заднего буферов (Double Buffering)

		glfwPollEvents(); // Опрос системной очереди событий (ввод, изменение геометрии окна)
	}

	chunkText.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window); // Уничтожение дескриптора окна
	glfwTerminate();           // Корректное завершение работы подсистемы GLFW
	return 0;
}