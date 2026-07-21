#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "Cube.h"
#include "shaderClass.h" // Подключаем ваш класс шейдера

class Chunk {
public:
    static constexpr int CHUNK_SIZE_X = 16;
    static constexpr int CHUNK_SIZE_Y = 30;
    static constexpr int CHUNK_SIZE_Z = 16;

    int chunkX;
    int chunkY;
    int chunkZ;

    int chunkData[16][30][16];

    std::vector<GLfloat> chunkVertices;
    unsigned int chunkVAO = 0, chunkVBO = 0;
    int totalVertices = 0;

    Chunk(int x, int y, int z)
    {
        chunkX = x;
        chunkY = y;
        chunkZ = z;
    }

    void FillChunkData() {
        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    if (y == 29) chunkData[x][y][z] = 1; // Трава
                    else if (y < 29 && y > 25) chunkData[x][y][z] = 2; //Земля
                    else chunkData[x][y][z] = 3; // Камень
                }
            }
        }
    }

    void AddCubeToChunkMesh(float offsetX, float offsetY, float offsetZ, int blockType) {
        const int floatsPerVertex = 5; // X, Y, Z, U, V
        const float textureSize = 1.0f / 6.0f;

        int totalIndices = sizeof(Cube::indices) / sizeof(GLuint);

        for (int i = 0; i < totalIndices; i++) {
            GLuint vertexIndex = Cube::indices[i];
            int vertexOffset = vertexIndex * floatsPerVertex;

            // Определяем, какую грань мы сейчас обрабатываем (всего 6 граней по 6 индексов)
            int faceIndex = i / 6;

            // Переменные для хранения текущего слота атласа
            int col = 3;
            int row = 3;

            if (blockType == 1)
            {
                // Верх
                if (faceIndex == 5)
                {
                    col = 0;
                    row = 0;
                }
                // Низ
                else if (faceIndex == 4)
                {
                    col = 1;
                    row = 0;
                }
                // Бока
                else
                {
                    col = 0;
                    row = 1;
                }
            }

            if (blockType == 2)
            {
                // Верх
                if (faceIndex == 5)
                {
                    col = 1;
                    row = 0;
                }
                // Низ
                else if (faceIndex == 4)
                {
                    col = 1;
                    row = 0;
                }
                // Бока
                else
                {
                    col = 1;
                    row = 0;
                }
            }

            if (blockType == 3)
            {
                // Верх
                if (faceIndex == 5)
                {
                    col = 0;
                    row = 2;
                }
                // Низ
                else if (faceIndex == 4)
                {
                    col = 0;
                    row = 2;
                }
                // Бока
                else
                {
                    col = 0;
                    row = 2;
                }
            }

            // Вычисляем смещение текстуры в пространстве OpenGL
            float uOffset = (float)row * textureSize;
            float vOffset = (5 - col) * textureSize;

            // 1. Координаты позиции (X, Y, Z)
            GLfloat finalX = Cube::vertices[vertexOffset] + offsetX;
            GLfloat finalY = Cube::vertices[vertexOffset + 1] + offsetY;
            GLfloat finalZ = Cube::vertices[vertexOffset + 2] + offsetZ;

            chunkVertices.push_back(finalX);
            chunkVertices.push_back(finalY);
            chunkVertices.push_back(finalZ);

            // 2. Текстурные координаты (U, V)
            GLfloat originalU = Cube::vertices[vertexOffset + 3];
            GLfloat originalV = Cube::vertices[vertexOffset + 4];

            // Масштабируем UV под рамки 1/9 атласа и сдвигаем в нужный квадрат
            GLfloat finalU = (originalU * textureSize) + uOffset;
            GLfloat finalV = (originalV * textureSize) + vOffset;

            chunkVertices.push_back(finalU);
            chunkVertices.push_back(finalV);
        }
    }



    void BuildChunkMesh() {
        chunkVertices.clear();

        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    if (chunkData[x][y][z] != 0) {
                        // Просто передаем координаты сетки! 
                        // Блок автоматически встанет в рамки от [x] до [x + 1]
                        AddCubeToChunkMesh((float)x, (float)y, (float)z, chunkData[x][y][z]);
                    }
                }
            }
        }

        totalVertices = chunkVertices.size() / 5;
    }


    void UploadChunkToGPU() {
        if (chunkVAO == 0) {
            glGenVertexArrays(1, &chunkVAO);
            glGenBuffers(1, &chunkVBO);
        }

        glBindVertexArray(chunkVAO);
        glBindBuffer(GL_ARRAY_BUFFER, chunkVBO);

        glBufferData(GL_ARRAY_BUFFER, chunkVertices.size() * sizeof(GLfloat), chunkVertices.data(), GL_STATIC_DRAW);

        // Атрибут 0: Позиция (X, Y, Z) - занимает 3 GLfloat
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

        // Атрибут 1: Текстура (U, V) - занимает 2 GLfloat, идет после первых 3-х
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }

    // ВАЖНО: Передаем shaderProgram по ссылке
    void DrawChunk(Shader& shader) {
        if (totalVertices == 0) return;

        shader.Activate();

        // СБРОС МАТРИЦЫ МОДЕЛИ:
        // Так как ваши блоки уже физически смещены внутри VBO буфера на свои места (x,y,z),
        // матрица model в шейдере должна быть строго единичной (identity matrix).
        // Если ее не передать, там может оказаться мусор или нули.
        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                chunkX * CHUNK_SIZE_X,
                chunkY * CHUNK_SIZE_Y,
                chunkZ * CHUNK_SIZE_Z
            ));

        // Находим локацию юниформа "model" в вашем шейдере
        GLint modelLoc = glGetUniformLocation(shader.ID, "model");
        if (modelLoc != -1) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        }

        glBindVertexArray(chunkVAO);
        glDrawArrays(GL_TRIANGLES, 0, totalVertices);
        glBindVertexArray(0);
    }

    ~Chunk() {
        if (chunkVAO != 0) glDeleteVertexArrays(1, &chunkVAO);
        if (chunkVBO != 0) glDeleteBuffers(1, &chunkVBO);
    }
};

#endif
