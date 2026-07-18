#ifndef TRANSFORM_CLASS_H
#define TRANSFORM_CLASS_H

#include<glad//glad.h>
#include<glm/glm.hpp>
#include<glm/gtc//matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

class Transform {
public:
	glm::vec3 position;

	void InitPos(GLuint ID);
};

#endif