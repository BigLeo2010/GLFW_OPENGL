#include"Texture.h"

// Конструктор класса: загружает изображение, создает текстурный объект и настраивает параметры
Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum pixelType) {
	// Сохраняем тип текстуры (например, GL_TEXTURE_2D) во внутреннюю переменную класса
	type = texType;

	// Переменные для хранения размеров изображения и количества цветовых каналов
	int widthImg, heightImg, numColCh;

	// Переворачиваем картинку по вертикали, так как в OpenGL начало координат (0,0) — снизу слева, а у картинок — сверху слева
	stbi_set_flip_vertically_on_load(true);

	// Загружаем пиксели из файла с помощью библиотеки stb_image
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// Генерируем 1 уникальный ID (хэндл) для текстуры и записываем его в ID класса
	glGenTextures(1, &ID);

	// Активируем нужный текстурный слот (например, GL_TEXTURE0)
	glActiveTexture(slot);

	// Привязываем (делаем активной) созданную текстуру в текущем контексте OpenGL
	glBindTexture(texType, ID);

	// Настройка фильтрации при уменьшении и увеличении текстуры (GL_NEAREST дает пиксельный ретро-стиль)
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Настройка поведения текстурных координат при выходе за пределы [0, 1] по горизонтали (S) и вертикали (T)
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Автоматически определяем формат пикселей в зависимости от количества каналов в файле
	GLenum format = GL_RGB; // По умолчанию предполагаем 3 канала (RGB, например для JPG)
	if (numColCh == 4) {
		format = GL_RGBA;   // 4 канала (RGBA, например для прозрачных PNG)
	}
	else if (numColCh == 1) {
		format = GL_RED;    // 1 канал (градации серого)
	}

	// Отключаем выравнивание строк по 4 байта. Спасает от диагонального перекоса текстур (например, 16x16)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Загружаем данные пикселей из оперативной памяти (массив bytes) в видеопамять видеокарты
	glTexImage2D(texType, 0, format, widthImg, heightImg, 0, format, pixelType, bytes);

	// Автоматически генерируем мипмапы (копии текстуры меньшего разрешения для оптимизации на расстоянии)
	glGenerateMipmap(texType);

	// Освобождаем память, которую выделила библиотека stb_image под массив пикселей
	stbi_image_free(bytes);

	// Отвязываем текстуру, чтобы случайно не изменить её параметры в другом месте кода
	glBindTexture(texType, 0);
}

// Привязка uniform-переменной (семплера) шейдера к конкретному порядковому номеру текстурного юнита
void Texture::texIUnit(Shader shader, const char* uniform, GLuint unit) {
	// Находим позицию uniform-переменной в скомпилированном шейдере по её имени (например, "tex0")
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);

	// Активируем шейдерную программу, перед тем как менять её переменные
	shader.Activate();

	// Передаем номер текстурного слота (индекс, например 0 для GL_TEXTURE0) в uniform-переменную
	glUniform1i(texUni, unit);
}

// Активация текстуры перед рендерингом объекта
void Texture::Bind() {
	glBindTexture(type, ID);
}

// Деактивация текстуры (сброс состояния)
void Texture::Unbind() {
	glBindTexture(type, 0);
}

// Удаление текстуры из памяти видеокарты при уничтожении объекта
void Texture::Delete() {
	glDeleteTextures(1, &ID);
}
