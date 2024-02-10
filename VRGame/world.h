#pragma once

#include <vector>
#include <iostream>
#include "perlin.h"
#include "chunk.h"
#include "shaders/shader.h"
#include "glm/glm.hpp"
#include <random>

#define WORLD_DEF


using namespace std;

class World {
public:
	World(int seed);
	//~World();
	int getHighestBlock(int x, int z);
	void setBlock(int x, int y, int z, uint8_t set);
	bool inWorld(float x, float y, float z);
	bool inWorld(glm::vec3 pos);
	bool inSolid(float x, float y, float z);
	bool inSolid(glm::vec3 pos);
	glm::vec3 mansp(float x, float y, float z); //Most Appropriate Non Solid Position
	glm::vec3 mansp(glm::vec3 pos);
	Chunk* getChunk(int cx, int cz);
	glm::vec3 getSkyColor();
	void update();
	void render(BasicShader* bs, glm::mat4 camera, glm::mat4 projection);
	int seed;
private:
	int tick;
	Perlin noiseGen;
	vector<Chunk*> loadedChunks;
};

