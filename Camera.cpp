#include"Camera.h"

// Реализация конструктора: инициализируем позицию и размеры экрана
Camera::Camera(int width, int height, glm::vec3 position) {
	Position = position;
	Camera::width = width;
	Camera::height = height;
}

// Расчет и передача MVP-матрицы в шейдер
void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform) {
	// Инициализируем единичные матрицы
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// lookAt создает матрицу вида на основе: где мы стоим, куда смотрим (Позиция + Направление) и где верх
	view = glm::lookAt(Position, Position + Orientation, Up);

	// perspective создает матрицу проекции. Приводим width и height к float, чтобы избежать целочисленного деления
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / (float)height, nearPlane, farPlane);

	// Передаем итоговую скомбинированную матрицу (проекция * вид) в uniform-переменную шейдера
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

// Обработка клавиш и перемещения мыши (ИЗМЕНЕНО: Добавлен аргумент float deltaTime)
void Camera::Inputs(GLFWwindow* window, float deltaTime) {

	// Ускорение при зажатом Left Shift (спринт)
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 10.0f; // ИЗМЕНЕНО: Значения увеличены, так как теперь они умножаются на секунды
	}
	// Возврат к обычной скорости, если Shift отпущен
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		speed = 5.0f; // ИЗМЕНЕНО: Значения увеличены
	}

	// ДОБАВЛЕНО: Расчет скорости для текущего кадра на основе deltaTime
	float currentSpeed = speed * deltaTime;

	// --- ДВИЖЕНИЕ НА КЛАВИАТУРУ ---

	// Движение ВПЕРЕД (по вектору направления взгляда)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Position += currentSpeed * Orientation; // ИЗМЕНЕНО: Заменено на currentSpeed
	}
	// Движение ВЛЕВО (перпендикулярно взгляду и верху). Используем векторное произведение (cross product)
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Position += currentSpeed * -glm::normalize(glm::cross(Orientation, Up)); // ИЗМЕНЕНО: Заменено на currentSpeed
	}
	// Движение НАЗАД (против вектора направления взгляда)
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Position += currentSpeed * -Orientation; // ИЗМЕНЕНО: Заменено на currentSpeed
	}
	// Движение ВПРАВО (перпендикулярно взгляду и верху)
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Position += currentSpeed * glm::normalize(glm::cross(Orientation, Up)); // ИЗМЕНЕНО: Заменено на currentSpeed
	}
	// Движение ВВЕРХ (строго по мировой оси Up)
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		Position += currentSpeed * Up; // ИЗМЕНЕНО: Заменено на currentSpeed
	}
	// Движение ВНИЗ (противно мировой оси Up)
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		Position += currentSpeed * -Up; // ИЗМЕНЕНО: Заменено на currentSpeed
	}

	// --- ПЕРЕКЛЮЧЕНИЕ КАМЕРЫ КНОПКОЙ O (TOGGLE) ---

	// Эти переменные должны быть объявлены вне цикла или быть static
	static bool isPressed = false;   // Запоминает, была ли кнопка зажата в прошлом кадре
	static bool canDoSmth = true;    // По умолчанию камера работает, курсор скрыт

	// Проверяем текущее физическое состояние кнопки O
	bool isNowPressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);

	// Если кнопку только что нажали (в прошлом кадре она НЕ была нажата)
	if (isNowPressed && !isPressed) {
		canDoSmth = !canDoSmth; // Инвертируем состояние (включаем/выключаем камеру)
		isPressed = true;       // Блокируем повторное срабатывание, пока кнопку удерживают
	}
	// Если кнопку полностью отпустили
	else if (!isNowPressed && isPressed) {
		isPressed = false;      // Сбрасываем блокировку, готовы к новому клику
	}

	if (canDoSmth) {
		// Режим обзора: скрываем курсор
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick) {
			glfwSetCursorPos(window, (width / 2.0), (height / 2.0));
			firstClick = false;
		}

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (height / 2.0));
		float rotY = sensitivity * (float)(mouseX - (width / 2.0));

		glm::vec3 newOrientation = glm::rotate(Orientation, -rotX, glm::normalize(glm::cross(Orientation, Up)));

		// Защита от переворота головы
		if (glm::angle(newOrientation, Up) > glm::radians(5.0f) && glm::angle(newOrientation, -Up) > glm::radians(5.0f)) {
			Orientation = newOrientation;
		}

		Orientation = glm::rotate(Orientation, -rotY, Up);

		// Возвращаем курсор обратно в центр
		glfwSetCursorPos(window, (width / 2.0), (height / 2.0));
	}
	else {
		// Режим курсора: возвращаем стандартную мышь
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// Важно: взводим флаг в true, чтобы при возврате в режим камеры 
		// не было резкого прыжка из-за старых координат мыши
		firstClick = true;
	}
}
