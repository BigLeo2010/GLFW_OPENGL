#include"Transform.h"

void Transform::InitPos(GLuint ID) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	GLuint modelLoc = glGetUniformLocation(ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}