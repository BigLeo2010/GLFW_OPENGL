#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>

class EBO {
public:
	GLuint ID; // Уникальный номер (паспорт) этого индексного буфера на видеокарте
	EBO(GLuint* indices, GLsizeiptr size); // Конструктор для создания и заполнения буфера индексами

	void Bind(); // Включить этот буфер индексов
	void Unbind(); // Выключить (отвязать) этот буфер индексов
	void Delete(); // Удалить буфер индексов из памяти видеокарты
};

#endif
