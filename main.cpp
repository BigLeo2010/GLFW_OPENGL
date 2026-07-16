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

// Интерливированный (interleaved) массив вершинных атрибутов: Позиция (vec3) и Цвет (vec3)
GLfloat vertices[] = {
	// Позиция (X, Y, Z)     // Цвет (R, G, B)        // Текстура (U, V)

	// Передняя грань (Z = 0.5)
	-0.5f, -0.5f,  0.5f,     1.0f, 1.0f, 0.0f,        0.0f, 0.0f, // 0
	 0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,        1.0f, 0.0f, // 1
	 0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        1.0f, 1.0f, // 2
	-0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 1.0f,        0.0f, 1.0f, // 3

	// Задняя грань (Z = -0.5)
	-0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        1.0f, 0.0f, // 4
	 0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,        0.0f, 0.0f, // 5
	 0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,        0.0f, 1.0f, // 6
	-0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 1.0f,        1.0f, 1.0f, // 7

	// Левая грань (X = -0.5)
	-0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        0.0f, 0.0f, // 8
	-0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,        1.0f, 0.0f, // 9
	-0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        1.0f, 1.0f, // 10
	-0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 1.0f,        0.0f, 1.0f, // 11

	// Правая грань (X = 0.5)
	 0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        1.0f, 0.0f, // 12
	 0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f,        0.0f, 0.0f, // 13
	 0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        0.0f, 1.0f, // 14
	 0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 1.0f,        1.0f, 1.0f, // 15

	 // Нижняя грань (Y = -0.5)
	 -0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        0.0f, 1.0f, // 16
	  0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,        1.0f, 1.0f, // 17
	  0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        1.0f, 0.0f, // 18
	 -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 1.0f,        0.0f, 0.0f, // 19

	 // Верхняя грань (Y = 0.5)
	 -0.5f,  0.5f, -0.5f,     1.0f, 1.0f, 0.0f,        0.0f, 0.0f, // 20
	  0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,        1.0f, 0.0f, // 21
	  0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,        1.0f, 1.0f, // 22
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

	float rotation = 0.0f;
	double prevTime = glfwGetTime();
	float speed = 1.5f;

	glEnable(GL_DEPTH_TEST);


	// Основной цикл обработки сообщений и рендеринга (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		// Заполнение буфера кадра заданным цветом (Clear Color Stage)
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Инвалидация и очистка цветового буфера

		shaderProgram.Activate(); // Инжект шейдерной программы в текущий пайплайн

		//АНИМАЦИЯ КУБА
		double crntTime = glfwGetTime();
		// Вычисляем дельту времени (сколько секунд прошло с прошлого кадра)
		float deltaTime = (float)(crntTime - prevTime);
		prevTime = crntTime;

		// Увеличиваем угол плавно. 45.0f — это скорость в градусах в секунду
		rotation += speed * 45.0f * deltaTime;

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		// Вращаем плавно по радианам, изменив ось на более красивую (например, X и Y)
		float time = (float)glfwGetTime();

		// Вычисляем динамические углы поворота для каждой оси (в радианах)
		float rotX = time * speed;
		float rotY = time * (speed * 0.7f) + std::sin(time * 0.5f); // Смещаем фазу синусом
		float rotZ = time * (speed * 0.4f) + std::cos(time * 0.8f); // Смещаем фазу косинусом

		// Последовательно вращаем куб по всем трем осям
		model = glm::rotate(model, rotX, glm::vec3(1.0f, 0.0f, 0.0f)); // Вращение вокруг X
		model = glm::rotate(model, rotY, glm::vec3(0.0f, 1.0f, 0.0f)); // Вращение вокруг Y
		model = glm::rotate(model, rotZ, glm::vec3(0.0f, 0.0f, 1.0f)); // Вращение вокруг Z

		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
		// Защита от деления на ноль при расчете Aspect Ratio
		float aspect = (HEIGHT > 0) ? (float)WIDTH / (float)HEIGHT : 1.0f;
		proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);


		int modelLocation = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		int viewLocation = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
		int projLocation = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));


		// Динамическое обновление стейта на основе встроенного высокоточного таймера GLFW
		float currentTime = glfwGetTime(); // Получение дельты времени в секундах

		glUniform1f(uniID, scale); // Передача скаляра масштабирования в uniform-регистр GPU
		glUniform1f(timeID, currentTime); // ДОБАВИЛИ: Передача значения времени для внутренней процедурной графики фрагментного шейдера
		grassBlock.Bind();

		VAO1.Bind(); // Контекстная активация сконфигурированных вершинных атрибутов

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
