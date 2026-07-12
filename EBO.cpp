#include"EBO.h"

// Конструктор класса
EBO::EBO(GLuint* indices, GLsizeiptr size) {
	glGenBuffers(1, &ID); // Генерируем EBO и записываем его ID
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); // Привязываем этот буфер как индексный
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW); // Отправляем массив индексов на видеокарту
}

// Метод для активации EBO
void EBO::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); // Делаем этот буфер индексов активным
}

// Метод для деактивации EBO
void EBO::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Отвязываем текущий буфер индексов (0 — сброс)
}

// Метод для очистки памяти
void EBO::Delete() {
	glDeleteBuffers(1, &ID); // Удаляем буфер индексов с видеокарты
}
