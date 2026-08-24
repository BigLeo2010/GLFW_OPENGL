#ifndef GAMEOBJECT_CLASS_H
#define GAMEOBJECT_CLASS_H

#include "shaderClass.h"
#include "Texture.h"
#include "Mesh.h"
#include "Transform.h"

class GameObject {
public:
	Mesh mesh;
	Texture texture;
	Transform transform;
	bool hasTexture;

	GameObject(const GLfloat* vertices, GLsizeiptr vertSize,
		const GLuint* indices, GLsizeiptr indSize,
		const char* texPath, Shader& shader);

	GameObject(const GLfloat* vertices, GLsizeiptr vertSize,
		const GLuint* indices, GLsizeiptr indSize,
		glm::vec3 RGB, Shader& shader);

	void DrawObject(Shader& shader); // Передаем объект шейдера, а не просто ID
	~GameObject();
};

#endif
