#include "world.h"

World::World(int seed) : seed(seed), noiseGen(seed), tick(0) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			loadedChunks.push_back(new Chunk(&noiseGen, i - 5, j - 5, seed, this));
		}
	}
}

//Returns the highest block or -1 if no loaded chunk contains the coordinates
int World::getHighestBlock(int x, int z) {
	int cx = x / CHUNK_SIZE;
	int cz = z / CHUNK_SIZE;
	for (Chunk* c : loadedChunks) {
		if (c->x == cx && c->z == cz) {
			return c->getHighestSolid(x - (cx * CHUNK_SIZE), z - (cz * CHUNK_SIZE));
		}
	}
	return -1;
}

void World::setBlock(int x, int y, int z, uint8_t set) {
	if (y >= CHUNK_SIZE || y < 0) {
		return;
	}
	int cx = floor((double)x / CHUNK_SIZE);
	int cz = floor((double)z / CHUNK_SIZE);
	for (Chunk* c : loadedChunks) {
		if (c->x == cx && c->z == cz) {
			int dx = x - cx * CHUNK_SIZE, dz = z - cz * CHUNK_SIZE;
			if (c->getData(dx, y, dz) != set) {
				c->setData(dx, y, dz, set);
				if (dx == 0) {
					getChunk(cx - 1, cz)->needsMeshUpdate = true;
				}
				if (dz == 0) {
					getChunk(cx, cz - 1)->needsMeshUpdate = true;
				}
			}
			return;
		}
	}
}

bool World::inWorld(float x, float y, float z) {
	return y > 0 && y < (CHUNK_SIZE * BLOCK_SIZE);
}

bool World::inWorld(glm::vec3 pos) {
	return inWorld(pos.x, pos.y, pos.z);
}

bool World::inSolid(float x, float y, float z) {
	if(!inWorld(x, y, z)) {
		return false;
	}
	int cx = floor(x / BLOCK_SIZE / CHUNK_SIZE);
	int cz = floor(z / BLOCK_SIZE / CHUNK_SIZE);
	Chunk* c = getChunk(cx, cz);
	return c->inSolid(x - cx * CHUNK_SIZE * BLOCK_SIZE, y, z - cz * CHUNK_SIZE * BLOCK_SIZE);
}

bool World::inSolid(glm::vec3 pos) {
	return inSolid(pos.x, pos.y, pos.z);
}

//Most Appropriate Non Solid Position (just checks upwards for now because testing)
glm::vec3 World::mansp(float x, float y, float z) {
	if (!inSolid(x, y, z)) {
		return glm::vec3(x, y, z);
	}
	float ny = (int)(y / 0.1f) * 0.1f;
	while (inWorld(x, ny, z)) {
		ny += 0.1f;
		if (!inSolid(x, ny, z)) {
			break;
		}
	}
	return glm::vec3(x, ny, z);
}

glm::vec3 World::mansp(glm::vec3 pos) {
	return mansp(pos.x, pos.y, pos.z);
}

Chunk* World::getChunk(int chunkX, int chunkZ) {
	for (Chunk* c : loadedChunks) {
		if (c->x == chunkX && c->z == chunkZ) {
			return c;
		}
	}
	return nullptr;
}

glm::vec3 World::getSkyColor() {
	float sky = max(cos(tick / 10000.0), 0.1);
	return glm::vec3(0.1f, 0.7f, 1.0f) * sky;
}

void World::update() {
	for (Chunk* c : loadedChunks) {
		c->update();
	}
	//tick++;
}

void World::render(BasicShader* bs, glm::mat4 camera, glm::mat4 projection) {
	float sky = max(cos(tick / 10000.0), 0.1);
	bs->loadSkyLight(glm::vec3(sky));
	for (Chunk* c : loadedChunks) {
		c->render(bs, camera, projection);
	}
}