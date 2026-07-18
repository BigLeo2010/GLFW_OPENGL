#include "GameObject.h"

// Конструктор: через двоеточие принудительно инициализируем текстуру переданными параметрами
GameObject::GameObject(const GLfloat* vertices, GLsizeiptr vertSize,
	const GLuint* indices, GLsizeiptr indSize,
	const char* texPath, Shader& shader)
	: texture(texPath, GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE)
{
	// Теперь mesh создает геометрию по ЧЕСТНЫМ размерам байт, переданным снаружи
	mesh.CreateMesh((GLfloat*)vertices, vertSize, (GLuint*)indices, indSize);

	// Привязываем uniform-переменную текстуры к шейдеру
	texture.texIUnit(shader, "tex0", 0);
}

void GameObject::DrawObject(Shader& shader) {
	texture.Bind(); // Теперь активируется реальная текстура объекта, а не пустая

	transform.InitPos(shader.ID);

	mesh.DrawMesh();
}

void GameObject::DeleteObject() {
	mesh.DeleteMesh();
	texture.Delete();
}
