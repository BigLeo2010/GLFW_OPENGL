#ifndef TRANSFORM_CLASS_H
#define TRANSFORM_CLASS_H

#include<glad//glad.h>
#include<glm/glm.hpp>
#include<glm/gtc//matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

class Transform {
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.0f,1.0f,1.0f);

	void InitTransform(GLuint ID);
};

#endif