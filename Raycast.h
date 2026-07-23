#ifndef RAYCAST_H
#define RAYCAST_H

#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include "Camera.h"
#include "Chunk.h"

class Raycast {
public:
    // Результат работы луча
    struct HitResult {
        bool hit = false;             // Был ли найден блок?
        glm::ivec3 blockPos{ 0 };       // Мировые координаты задетого блока
        glm::ivec3 placePos{ 0 };       // Мировые координаты места установки нового блока
        Chunk* targetChunk = nullptr; // Чанк с задетым блоком
        Chunk* placeChunk = nullptr;  // Чанк для установки нового блока
    };

    // Главный метод пускания луча
    static HitResult Trace(Camera& camera, std::vector<Chunk>& world, float maxDistance = 6.0f, float step = 0.01f) {
        HitResult result;

        // Позиция и направление взгляда игрока
        // Убедись, что в твоем классе Camera эти поля публичные (camera.Position и camera.Orientation)
        glm::vec3 rayOrigin = camera.Position;
        glm::vec3 rayDir = camera.Orientation;

        // Изначальный целочисленный воксель, где стоит камера
        glm::ivec3 lastIntegerPos = glm::ivec3(
            std::floor(rayOrigin.x),
            std::floor(rayOrigin.y),
            std::floor(rayOrigin.z)
        );

        glm::vec3 currentPos = rayOrigin;

        // Смещаем старт луча на 0.1 вперед, чтобы не попадать в блок, если стоим к нему вплотную
        currentPos += rayDir * 0.1f;

        for (float t = 0.1f; t < maxDistance; t += step) {
            currentPos += rayDir * step;

            glm::ivec3 currentIntegerPos = glm::ivec3(
                std::floor(currentPos.x),
                std::floor(currentPos.y),
                std::floor(currentPos.z)
            );

            // Если луч перешагнул в новую ячейку воксельной сетки
            if (currentIntegerPos != lastIntegerPos) {

                // Переводим мировые координаты луча в мировые координаты сетки Чанков
                int cx = std::floor((float)currentIntegerPos.x / (float)Chunk::CHUNK_SIZE_X);
                int cz = std::floor((float)currentIntegerPos.z / (float)Chunk::CHUNK_SIZE_Z);

                // Защита по высоте: твой мир ограничен по Y от 0 до 60
                if (currentIntegerPos.y >= 0 && currentIntegerPos.y < Chunk::CHUNK_SIZE_Y) {

                    Chunk* hitChunk = FindChunk(world, cx, cz);

                    if (hitChunk != nullptr) {
                        // Перевод в локальные координаты чанка (0...15)
                        int localX = currentIntegerPos.x - (cx * Chunk::CHUNK_SIZE_X);
                        int localY = currentIntegerPos.y;
                        int localZ = currentIntegerPos.z - (cz * Chunk::CHUNK_SIZE_Z);

                        // Проверяем, есть ли блок в этой локальной позиции
                        if (hitChunk->HasBlockAt(localX, localY, localZ)) {

                            result.hit = true;
                            result.blockPos = currentIntegerPos;
                            result.targetChunk = hitChunk;

                            // Место для установки нового блока — это координаты ПРЕДЫДУЩЕГО шага луча
                            result.placePos = lastIntegerPos;

                            // Находим чанк для установки нового блока (он может оказаться соседним!)
                            int pcx = std::floor((float)lastIntegerPos.x / (float)Chunk::CHUNK_SIZE_X);
                            int pcz = std::floor((float)lastIntegerPos.z / (float)Chunk::CHUNK_SIZE_Z);
                            result.placeChunk = FindChunk(world, pcx, pcz);

                            return result; // Нашли пересечение, прерываем цикл луча
                        }
                    }
                }

                // Запоминаем текущий воксель как "прошлый пустой"
                lastIntegerPos = currentIntegerPos;
            }
        }

        return result; // Возвращаем hit = false
    }

private:
    // Поиск чанка в векторе по координатам сетки
    static Chunk* FindChunk(std::vector<Chunk>& world, int cx, int cz) {
        for (auto& chunk : world) {
            if (chunk.GetX() == cx && chunk.GetZ() == cz) {
                return &chunk;
            }
        }
        return nullptr;
    }
};

#endif
