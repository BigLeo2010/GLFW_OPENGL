#ifndef BLOCK_REGISTRY_H
#define BLOCK_REGISTRY_H

#include <map>
#include "GameObject.h"
#include "BlockType.h"
#include "Cube.h"

class BlockRegistry {
private:
    // Хранилище «образцов» блоков. Ключ — тип блока, значение — сам объект
    static inline std::map<BlockType, GameObject*> blockTemplates;

public:
    // Загружаем все блоки один раз при старте
    static void Initialize(Shader& shader) {
        // Регистрируем траву
        blockTemplates[BlockType::GRASS] = new GameObject(
            Cube::vertices, sizeof(Cube::vertices),
            Cube::indices, sizeof(Cube::indices),
            "grass_text.png", shader
        );

        // Регистрируем камень (для примера)
        blockTemplates[BlockType::STONE] = new GameObject(
            Cube::vertices, sizeof(Cube::vertices),
            Cube::indices, sizeof(Cube::indices),
            "stone_text.png", shader
        );

        blockTemplates[BlockType::WOOD] = new GameObject(
            Cube::vertices, sizeof(Cube::vertices),
            Cube::indices, sizeof(Cube::indices),
            "wood_text.png", shader
        );

        blockTemplates[BlockType::DIRT] = new GameObject(
            Cube::vertices, sizeof(Cube::vertices),
            Cube::indices, sizeof(Cube::indices),
            "dirt_text.png", shader
        );

        blockTemplates[BlockType::SAND] = new GameObject(
            Cube::vertices, sizeof(Cube::vertices),
            Cube::indices, sizeof(Cube::indices),
            "sand_text.png", shader
        );

        blockTemplates[BlockType::DIAMOND_ORE] = new GameObject(
            Cube::vertices, sizeof(Cube::vertices),
            Cube::indices, sizeof(Cube::indices),
            "diamond_ore_text.png", shader
        );

        blockTemplates[BlockType::IRON_ORE] = new GameObject(
            Cube::vertices, sizeof(Cube::vertices),
            Cube::indices, sizeof(Cube::indices),
            "iron_ore_text.png", shader
        );
    }

    // Метод «Фабрика» — создает копию блока в определенной позиции
    static GameObject CreateBlock(BlockType type, glm::vec3 position) {
        // Берем шаблон из базы данных
        GameObject* templateObj = blockTemplates[type];

        // Создаем новый объект на основе шаблона (копируем меш и текстуру)
        GameObject newBlock = *templateObj;
        newBlock.transform.position = position; // Задаем ему индивидуальное место

        return newBlock;
    }

    // Очистка памяти в конце игры
    static void Clear() {
        for (auto& pair : blockTemplates) {
            pair.second->DeleteObject();
            delete pair.second;
        }
        blockTemplates.clear();
    }
};

#endif
