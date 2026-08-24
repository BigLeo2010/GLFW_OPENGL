#include "GameObject.h"

// Конструктор: через двоеточие принудительно инициализируем текстуру переданными параметрами
GameObject::GameObject(const GLfloat* vertices, GLsizeiptr vertSize,
	const GLuint* indices, GLsizeiptr indSize,
	const char* texPath, Shader& shader)
	: texture(texPath, GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE)
{
	mesh.CreateMesh((GLfloat*)vertices, vertSize, (GLuint*)indices, indSize);

	texture.texIUnit(shader, "tex0", 0);

	hasTexture = true;
}

GameObject::GameObject(const GLfloat* vertices, GLsizeiptr vertSize,
	const GLuint* indices, GLsizeiptr indSize,
	glm::vec3 RGB, Shader& shader){

	mesh.CreateMesh((GLfloat*)vertices, vertSize, (GLuint*)indices, indSize);

	GLuint colorID = glGetUniformLocation(shader.ID, "color");

	shader.Activate();

	glUniform3f(colorID, RGB.x, RGB.y, RGB.z);

	hasTexture = false;
}

void GameObject::DrawObject(Shader& shader) {
	if (hasTexture) texture.Bind();

	transform.InitTransform(shader.ID);

	mesh.DrawMesh();
}

GameObject::~GameObject() {
	mesh.DeleteMesh();
	if (hasTexture) texture.Delete();
}
