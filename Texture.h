#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"

// Класс для управления текстурами в OpenGL (генерация, привязка и байндинг к шейдерам)
class Texture {
public:
	GLuint ID;    // Уникальный идентификатор (хэндл) текстурного объекта в OpenGL
	GLenum type;  // Тип текстуры (например, GL_TEXTURE_2D, GL_TEXTURE_3D)

	/**
	 * Конструктор класса Texture
	 * @param image     - Путь к файлу изображения (например, "grass.jpg")
	 * @param texType   - Тип текстуры (обычно GL_TEXTURE_2D)
	 * @param slot      - Текстурный слот/юнит, в который загружается текстура (например, GL_TEXTURE0)
	 * @param pixelType - Тип данных пикселей изображения (обычно GL_UNSIGNED_BYTE)
	 */
	Texture(const char* image, GLenum texType, GLenum slot, GLenum pixelType);

	/**
	 * Связывает текстурный юнит (slot) с uniform-переменной (семплером) в шейдере
	 * @param shader  - Объект шейдерной программы
	 * @param uniform - Имя uniform-переменной в GLSL коде (например, "tex0")
	 * @param unit    - Порядковый номер текстурного слота (0 для GL_TEXTURE0, 1 для GL_TEXTURE1 и т.д.)
	 */
	void texIUnit(Shader shader, const char* uniform, GLuint unit);

	// Делает эту текстуру активной (привязывает к текущему контексту OpenGL)
	void Bind();

	// Отвязывает текстуру (сбрасывает состояние во избежание случайных изменений)
	void Unbind();

	// Удаляет текстурный объект из памяти видеокарты
	void Delete();
};

#endif
