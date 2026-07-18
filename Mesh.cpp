#include"Mesh.h"

void Mesh::CreateMesh(GLfloat vertices[], GLsizeiptr vertSize, GLuint indices[], GLsizeiptr indSize) {
	// Генерация Vertex Array Object (хранилище состояний вершинных атрибутов)
	VAO1.Bind();  // Активация VAO для записи последующих конфигураций буферов

	indexCount = indSize / sizeof(GLuint);

	// Выделение VRAM и аллокация данных (копирование массивов в видеопамять)
	VBO1 = new VBO(vertices, vertSize); // Аллокация GL_ARRAY_BUFFER
	EBO1 = new EBO(indices, indSize);   // Аллокация GL_ELEMENT_ARRAY_BUFFER (автоматически привязывается к текущему VAO)

	// Описание макета данных (Layout) для вершинных атрибутов внутри VAO:
	// Атрибут 0 (Координаты): vec3, шаг 32 байта, смещение 0 байт
	VAO1.LinkAttrib(*VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);

	// Атрибут 1 (Цвет): vec3, шаг 32 байта, смещение 3 * 4 = 12 байт
	VAO1.LinkAttrib(*VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	// Атрибут 2 (Текстурные координаты): vec2, шаг 32 байта, смещение 6 * 4 = 24 байта
	VAO1.LinkAttrib(*VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));


	VAO1.Unbind(); // Сброс состояния VAO (защита от непреднамеренной мутации стейта)
	VBO1->Unbind(); // Развязка GL_ARRAY_BUFFER
	EBO1->Unbind(); // Развязка GL_ELEMENT_ARRAY_BUFFER
}

void Mesh::DrawMesh() {
	VAO1.Bind();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	VAO1.Unbind();
}

void Mesh::DeleteMesh() {
	VAO1.Delete();
	VBO1->Delete();
	EBO1->Delete();
}