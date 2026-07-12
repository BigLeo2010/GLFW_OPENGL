#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno> // Нужен для получения кодов системных ошибок при чтении файлов

// Объявляем внешнюю функцию для чтения файлов, чтобы её можно было использовать
std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint ID; // Уникальный номер (паспорт) этой шейдерной программы на видеокарте
	Shader(const char* vertexFile, const char* fragmentFile); // Конструктор, принимающий пути к файлам шейдеров

	void Activate(); // Включить эту шейдерную программу для рендеринга
	void Delete(); // Удалить шейдерную программу из памяти видеокарты
private:
	void compileErrors(unsigned int shader, const char* type);
};

#endif
