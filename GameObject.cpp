#include "GameObject.h"

// Конструктор: через двоеточие принудительно инициализируем текстуру переданными параметрами
GameObject::GameObject(const GLfloat* vertices, GLsizeiptr vertSize,
	const GLuint* indices, GLsizeiptr indSize,
	const char* texPath, Shader& shader)
	: texture(texPath, GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE)
{
	mesh.CreateMesh((GLfloat*)vertices, vertSize, (GLuint*)indices, indSize);

	texture.texIUnit(shader, "tex0", 0);
}

void GameObject::DrawObject(Shader& shader) {
	texture.Bind();

	transform.InitTransform(shader.ID);

	mesh.DrawMesh();
}

void GameObject::DeleteObject() {
	mesh.DeleteMesh();
	texture.Delete();
}
