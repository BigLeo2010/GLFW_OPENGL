#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<glad/glad.h>
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

class Mesh {
public:
	VAO VAO1;
	VBO* VBO1;
	EBO* EBO1;
	int indexCount;

	void CreateMesh(GLfloat vertices[], GLsizeiptr vertSize, GLuint indices[], GLsizeiptr indSize);
	void DrawMesh();
	void DeleteMesh();
};

#endif