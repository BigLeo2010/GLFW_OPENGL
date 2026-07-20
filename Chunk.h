#ifndef CHUNK_H
#define CHUNK_H

#include<glm/glm.hpp>
#include"BlockRegistry.h"
#include<cstdlib>

class Chunk {
private:
	static inline std::vector<GameObject> chunkBlocks;
public:
	static void CreateChunk(glm::vec3 scale, glm::vec3 layout) {
		for (float x = 0; x < scale.x; x++) {
			for (float y = 0; y < scale.y; y++) {
				for (float z = 0; z < scale.z; z++) {
					if (-z > -4 && -z != 0)
					{
						chunkBlocks.push_back(BlockRegistry::CreateBlock(BlockType::DIRT, glm::vec3(x, -z, y) + layout));
					}
					else if (-z == 0) {
						chunkBlocks.push_back(BlockRegistry::CreateBlock(BlockType::GRASS, glm::vec3(x, -z, y) + layout));
					}
					else {
						int randomNumber = rand();
						if ((randomNumber % 20 + 1) != 20)
						{
							chunkBlocks.push_back(BlockRegistry::CreateBlock(BlockType::STONE, glm::vec3(x, -z, y) + layout));
						}
						else {
							chunkBlocks.push_back(BlockRegistry::CreateBlock(BlockType::DIAMOND_ORE, glm::vec3(x, -z, y) + layout));
						}
					}
				}
			}
		}
	}

	static void RenderChunk(Shader& shader) {
		for (int i = 0; i < chunkBlocks.size(); i++) {
			chunkBlocks[i].DrawObject(shader);
		}
	}
};

#endif