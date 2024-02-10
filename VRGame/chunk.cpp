#include "chunk.h"

#include "world.h"
#include <chrono>
#include <iostream>

#define time std::chrono::high_resolution_clock::now()
#define micros(x) std::chrono::duration_cast<std::chrono::microseconds>(x).count()
#define millis(x) std::chrono::duration_cast<std::chrono::milliseconds>(x).count()

Chunk::Chunk(Perlin* perlin, int x, int z, int seed, World* world) : x(x), z(z), world(world), chunkSeed(seed * x - z) {
	default_random_engine random(chunkSeed);
	data = new uint8_t ** [CHUNK_SIZE];
	uint8_t*** dataExtra = new uint8_t ** [CHUNK_SIZE + 1];
	dataExtra[0] = new uint8_t * [CHUNK_SIZE];
	for (int j = 0; j < CHUNK_SIZE; j++) {
		dataExtra[0][j] = new uint8_t[CHUNK_SIZE + 1];
	}
	for (int i = 0; i < CHUNK_SIZE; i++) {
		data[i] = new uint8_t *[CHUNK_SIZE];
		dataExtra[i + 1] = new uint8_t * [CHUNK_SIZE];
		for (int j = 0; j < CHUNK_SIZE; j++) {
			data[i][j] = new uint8_t[CHUNK_SIZE];
			for (int j = 0; j < CHUNK_SIZE; j++) {
				dataExtra[i + 1][j] = new uint8_t[CHUNK_SIZE + 1];
			}
		}
	}
	for (int i = 0; i < CHUNK_SIZE + 1; i++) {
		for (int k = 0; k < CHUNK_SIZE + 1; k++) {
			int height = (int)(perlin->noise((x * CHUNK_SIZE + i) * BLOCK_SIZE / WORLD_SCALE, (z * CHUNK_SIZE + k) * BLOCK_SIZE / WORLD_SCALE) * (CHUNK_SIZE / 2)) * 0.3 + (CHUNK_SIZE / 2);
			for (int j = 0; j < CHUNK_SIZE; j++) {
				//dataExtra[i][j][k] = j < height ? random() % 2 + 1 : 0;
				dataExtra[i][j][k] = j < height ? 1 : 0;
				if (i < CHUNK_SIZE && k < CHUNK_SIZE) {
					data[i][j][k] = dataExtra[i][j][k];
				}
			}
		}
	}
	int len = 0;
	MeshVert* out = cubeMarch(dataExtra, len);
	float** poop = trisToFloats(out, len);
	delete[] out;
	mesh = new Model(poop[0], len * 3, poop[1], true);
	delete[] poop[0];
	delete[] poop[1];
	delete[] poop;
	needsMeshUpdate = false;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE; j++) {
			delete[] dataExtra[i][j];
		}
		delete[] dataExtra[i];
	}
	delete[] dataExtra;
}

Chunk::~Chunk() {
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE; j++) {
			delete[] data[i][j];
		}
		delete[] data[i];
	}
	delete[] data;
	delete mesh;
}

uint8_t Chunk::getData(int x, int y, int z) {
	return data[x][y][z];
}

void Chunk::setData(int x, int y, int z, uint8_t set) {
	data[x][y][z] = set;
	needsMeshUpdate = true;
}

void Chunk::update() {
	if (needsMeshUpdate) {
		Chunk* xUp = world->getChunk(x + 1, z);
		Chunk* zUp = world->getChunk(x, z + 1);
		Chunk* xzUp = world->getChunk(x + 1, z + 1);
		uint8_t*** dataExtra = new uint8_t ** [CHUNK_SIZE + 1];
		for (int i = 0; i < CHUNK_SIZE + 1; i++) {
			dataExtra[i] = new uint8_t * [CHUNK_SIZE];
			for (int j = 0; j < CHUNK_SIZE; j++) {
				dataExtra[i][j] = new uint8_t[CHUNK_SIZE + 1];
				for (int k = 0; k < CHUNK_SIZE + 1; k++) {
					if (i < CHUNK_SIZE && k < CHUNK_SIZE) {
						dataExtra[i][j][k] = data[i][j][k];
					}
					else if (i >= CHUNK_SIZE && k < CHUNK_SIZE) {
						dataExtra[i][j][k] = xUp->data[0][j][k];
					}
					else if (i < CHUNK_SIZE && k >= CHUNK_SIZE) {
						dataExtra[i][j][k] = zUp->data[i][j][0];
					}
					else if (i >= CHUNK_SIZE && k >= CHUNK_SIZE) {
						dataExtra[i][j][k] = xzUp->data[0][j][0];
					}
				}
			}
		}
		int len = 0;
		MeshVert* out = cubeMarch(dataExtra, len);
		float** poop = trisToFloats(out, len);
		delete[] out;
		delete mesh;
		mesh = new Model(poop[0], len * 3, poop[1], true);
		delete[] poop[0];
		delete[] poop[1];
		delete[] poop;
		needsMeshUpdate = false;
		for (int i = 0; i < CHUNK_SIZE + 1; i++) {
			for (int j = 0; j < CHUNK_SIZE; j++) {
				delete[] dataExtra[i][j];
			}
			delete[] dataExtra[i];
		}
		delete[] dataExtra;
	}
}

bool Chunk::inSolid(float x, float y, float z) {
	return data[(int)((x) / BLOCK_SIZE)][(int)((y) / BLOCK_SIZE)][(int)((z) / BLOCK_SIZE)];
}

//struct MeshTri** Chunk::loadLookup() {
//	ifstream file;
//	file.open("C:/Users/Admin/Desktop/triangularsign.txt");
//	streampos start = file.tellg();
//	file.seekg(0, ios::end);
//	streampos end = file.tellg();
//	string buildNum = "";
//	int line = 0;
//	int tri = 0;
//	int r = 0;
//	file.seekg(start);
//	while ((r = file.get()) != '}') {
//		if (r == '(') {
//			tri++;
//		}
//	}
//	file.seekg(start);
//	MeshTri build = { -1, -1, -1, 0, 0, 0, -1, -1, -1, 0, 0, 0 };
//	MeshTri** lookup = new MeshTri*[256];
//	for (int i = 0; i < 256; i++) {
//		lookup[i] = new MeshTri[tri];
//	}
//	tri = 0;
//	int count = 0;
//	while (file.tellg() != -1) {
//		r = file.get();
//		//if (count % 100 == 0) {
//		//	cout << file.tellg() << " " << end << endl;
//		//}
//		//count++;
//		if (r == ')' || r == ',') {
//			if (build.sourceX == -1) {
//				build.sourceX = stoi(buildNum);
//			}
//			else if (build.sourceY == -1) {
//				build.sourceY = stoi(buildNum);
//			}
//			else if (build.sourceZ == -1) {
//				build.sourceZ = stoi(buildNum);
//			}
//			else if (build.p1 == -1) {
//				build.p1 = stoi(buildNum);
//			}
//			else if (build.p2 == -1) {
//				build.p2 = stoi(buildNum);
//			}
//			else if (build.p3 == -1) {
//				build.p3 = stoi(buildNum);
//			}
//			buildNum = "";
//			if (r == ')') {
//				lookup[line][tri] = build;
//				tri++;
//				build = { -1, -1, -1, 0, 0, 0, -1, -1, -1, 0, 0, 0 };
//				if (file.get() == '}') {
//					line++;
//					tri = 0;
//					file.get();
//					continue;
//				}
//			}
//		}
//		else if (r != '{' && r != '}' && r != '(') {
//			buildNum += (char)r;
//		}
//
//	}
//	file.close();
//	return lookup;
//}

int Chunk::lookup[256][16] = { {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
	{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
	{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
	{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
	{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
	{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
	{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
	{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
	{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
	{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
	{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
	{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
	{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
	{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
	{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
	{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
	{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
	{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
	{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
	{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
	{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
	{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
	{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
	{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
	{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
	{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
	{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
	{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
	{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
	{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
	{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
	{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
	{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
	{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
	{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
	{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
	{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
	{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
	{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
	{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
	{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
	{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
	{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
	{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
	{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
	{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
	{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
	{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
	{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
	{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
	{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
	{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
	{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
	{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
	{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
	{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
	{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
	{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
	{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
	{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
	{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
	{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
	{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
	{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
	{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
	{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
	{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
	{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
	{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
	{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
	{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
	{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
	{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
	{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
	{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
	{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
	{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
	{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
	{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
	{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
	{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
	{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
	{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
	{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
	{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
	{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
	{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
	{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
	{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
	{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
	{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} };

struct MeshVert Chunk::vertices[12] = {
				{0.5f, 0.0f, 0.0f},//0
				{1.0f, 0.5f, 0.0f},//1
				{0.5f, 1.0f, 0.0f},//2
				{0.0f, 0.5f, 0.0f},//3
				{0.5f, 0.0f, 1.0f},//4
				{1.0f, 0.5f, 1.0f},//5
				{0.5f, 1.0f, 1.0f},//6
				{0.0f, 0.5f, 1.0f},//7
				{0.0f, 0.0f, 0.5f},//8
				{1.0f, 0.0f, 0.5f},//9
				{1.0f, 1.0f, 0.5f},//10
				{0.0f, 1.0f, 0.5f}//11
//				{0.5f, 0.5f, 0.5f}//12
//				{1.0f, 0.5f, 0.5f},
//				{0.0f, 0.5f, 0.5f},
//				{0.5f, 1.0f, 0.5f},
//				{0.5f, 0.0f, 0.5f},
//				{0.5f, 0.5f, 1.0f},
//				{0.5f, 0.5f, 0.0f},
//				{1.0f, 0.25f, 0.25f},
//				{1.0f, 0.75f, 0.25f},
//				{1.0f, 0.25f, 0.75f},
//				{1.0f, 0.75f, 0.75f},
//				{0.0f, 0.25f, 0.25f},
//				{0.0f, 0.75f, 0.25f},
//				{0.0f, 0.25f, 0.75f},
//				{0.0f, 0.75f, 0.75f},
//				{0.25f, 1.0f, 0.25f},
//				{0.75f, 1.0f, 0.25f},
//				{0.25f, 1.0f, 0.75f},
//				{0.75f, 1.0f, 0.75f},
//				{0.25f, 0.0f, 0.25f},
//				{0.75f, 0.0f, 0.25f},
//				{0.25f, 0.0f, 0.75f},
//				{0.75f, 0.0f, 0.75f},
//				{0.25f, 0.25f, 1.0f},
//				{0.75f, 0.25f, 1.0f},
//				{0.25f, 0.75f, 1.0f},
//				{0.75f, 0.75f, 1.0f},
//				{0.25f, 0.25f, 0.0f},
//				{0.75f, 0.25f, 0.0f},
//				{0.25f, 0.75f, 0.0f},
//				{0.75f, 0.75f, 0.0f},
//				{0.5f, 0.25f, 0.25f},
//				{0.5f, 0.25f, 0.75f},
//				{0.5f, 0.75f, 0.25f},
//				{0.5f, 0.75f, 0.75f},
//				{0.25f, 0.5f, 0.25f},
//				{0.25f, 0.5f, 0.75f},
//				{0.75f, 0.5f, 0.25f},
//				{0.75f, 0.5f, 0.75f},
//				{0.25f, 0.25f, 0.5f},
//				{0.25f, 0.75f, 0.5f},
//				{0.75f, 0.25f, 0.5f},
//				{0.75f, 0.75f, 0.5f},
//				{1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f},
//				{1.0f / 6.0f, 1.0f / 6.0f, 5.0f / 6.0f},
//				{1.0f / 6.0f, 5.0f / 6.f, 1.f / 6.f},
//				{1.f / 6.f, 5.f / 6.f, 5.f / 6.f},
//				{5.f / 6.f, 1.f / 6.f, 1.f / 6.f},
//				{5.f / 6.f, 1.f / 6.f, 5.f / 6.f},
//				{5.f / 6.f, 5.f / 6.f, 1.f / 6.f},
//				{5.f / 6.f, 5.f / 6.f, 5.f / 6.f},
//				{0.75f, 0.5f, 0.5f},
//				{0.25f, 0.5f, 0.5f},
//				{0.5f, 0.75f, 0.5f},
//				{0.5f, 0.25f, 0.5f},
//				{0.5f, 0.5f, 0.75f},
//				{0.5f, 0.5f, 0.25f},
//				{5.f / 6.f, 0.5f, 0.5f},
//				{1.f / 6.f, 0.5f, 0.5f},
//				{0.5f, 5.f / 6.f, 0.5f},
//				{0.5f, 1.f / 6.f, 0.5f},
//				{0.5f, 0.5f, 5.f / 6.f},
//				{0.5f, 0.5f, 1.f / 6.f},
//				{5.f / 8.f, 5.f / 8.f, 0.75f},
//				{5.f / 8.f, 5.f / 8.f, 0.25f},
//				{5.f / 8.f, 3.f / 8.f, 0.75f},
//				{5.f / 8.f, 3.f / 8.f, 0.25f},
//				{3.f / 8.f, 5.f / 8.f, 0.75f},
//				{3.f / 8.f, 5.f / 8.f, 0.25f},
//				{3.f / 8.f, 3.f / 8.f, 0.75f},
//				{0.25f, 3.f / 8.f, 3.f / 8.f},
//				{0.25f, 3.f / 8.f, 5.f / 8.f},
//				{0.25f, 5.f / 8.f, 3.f / 8.f},
//				{0.25f, 5.f / 8.f, 5.f / 8.f},
//				{0.75f, 3.f / 8.f, 3.f / 8.f},
//				{0.75f, 3.f / 8.f, 5.f / 8.f},
//				{0.75f, 5.f / 8.f, 3.f / 8.f},
//				{0.75f, 5.f / 8.f, 5.f / 8.f},
//				{3.f / 8.f, 0.25f, 3.f / 8.f},
//				{3.f / 8.f, 0.25f, 5.f / 8.f},
//				{3.f / 8.f, 0.75f, 3.f / 8.f},
//				{3.f / 8.f, 0.75f, 5.f / 8.f},
//				{5.f / 8.f, 0.25f, 3.f / 8.f},
//				{5.f / 8.f, 0.25f, 5.f / 8.f},
//				{5.f / 8.f, 0.75f, 3.f / 8.f},
//				{5.f / 8.f, 0.75f, 5.f / 8.f}
};

void Chunk::render(BasicShader* bs, glm::mat4 camera, glm::mat4 projection) {
	bs->loadCamera(camera);
	bs->loadProjection(projection);
	glm::mat4 transform = glm::translate(glm::vec3(x * CHUNK_SIZE * BLOCK_SIZE, 0, z * CHUNK_SIZE * BLOCK_SIZE));
	bs->loadTransform(transform, 0.1);
	mesh->render();
}

int Chunk::getHighestSolid(int x, int z) {
	for(int i = CHUNK_SIZE - 1; i >= 0; i--) {
		if (data[x][i][z]) {
			return i;
		}
	}
	return -1;
}

MeshVert* Chunk::cubeMarch(uint8_t*** data, int& len) {
	uint8_t*** indices = new uint8_t**[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++) {
		indices[i] = new uint8_t*[CHUNK_SIZE - 1];
		for (int j = 0; j < CHUNK_SIZE - 1; j++) {
			indices[i][j] = new uint8_t[CHUNK_SIZE];
		}
	}

	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE - 1; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				uint8_t c = 0x0;
				if (data[i][j][k] > 0) {
					c |= 0x1;
				}
				if (data[i + 1][j][k] > 0) {
					c |= 0x2;
				}
				if (data[i + 1][j + 1][k] > 0) {
					c |= 0x4;
				}
				if (data[i][j + 1][k] > 0) {
					c |= 0x8;
				}
				if (data[i][j][k + 1] > 0) {
					c |= 0x10;
				}
				if (data[i + 1][j][k + 1] > 0) {
					c |= 0x20;
				}
				if (data[i + 1][j + 1][k + 1] > 0) {
					c |= 0x40;
				}
				if (data[i][j + 1][k + 1] > 0) {
					c |= 0x80;
				}
				indices[i][j][k] = c;
			}
		}
	}

	len = 0;

	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE - 1; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				int l = 0;
				while (l < 17 && lookup[indices[i][j][k]][l] != -1) {
					l++;
				}
				len += l;
			}
		}
	}

	int tri = 0;
	MeshVert* tris = new MeshVert[len];

	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE - 1; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				int l = 0;
				while (l < 17 && lookup[indices[i][j][k]][l] != -1) {
					//tris[tri] = vertices[lookup[indices[i][j][k]][l]][0] * BLOCK_SIZE + (i * BLOCK_SIZE);
					//tris[tri + 1] = vertices[lookup[indices[i][j][k]][l]][1] * BLOCK_SIZE + (j * BLOCK_SIZE);
					//tris[tri + 2] = vertices[lookup[indices[i][j][k]][l]][2] * BLOCK_SIZE + (k * BLOCK_SIZE);
					//tris[tri + 3] = vertices[lookup[indices[i][j][k]][l + 1]][0] * BLOCK_SIZE + (i * BLOCK_SIZE);
					//tris[tri + 4] = vertices[lookup[indices[i][j][k]][l + 1]][1] * BLOCK_SIZE + (j * BLOCK_SIZE);
					//tris[tri + 5] = vertices[lookup[indices[i][j][k]][l + 1]][2] * BLOCK_SIZE + (k * BLOCK_SIZE);
					//tris[tri + 6] = vertices[lookup[indices[i][j][k]][l + 2]][0] * BLOCK_SIZE + (i * BLOCK_SIZE);
					//tris[tri + 7] = vertices[lookup[indices[i][j][k]][l + 2]][1] * BLOCK_SIZE + (j * BLOCK_SIZE);
					//tris[tri + 8] = vertices[lookup[indices[i][j][k]][l + 2]][2] * BLOCK_SIZE + (k * BLOCK_SIZE);
					tris[tri] = vertices[lookup[indices[i][j][k]][l]];
					/*tris[tri].r = colorLookup[data[i + tris[tri].sourceX][j + tris[tri].sourceY][k + tris[tri].sourceZ]][0];
					tris[tri].g = colorLookup[data[i + tris[tri].sourceX][j + tris[tri].sourceY][k + tris[tri].sourceZ]][1];
					tris[tri].b = colorLookup[data[i + tris[tri].sourceX][j + tris[tri].sourceY][k + tris[tri].sourceZ]][2];*/
					float ix = i + tris[tri].x;
					float iy = j + tris[tri].y;
					float iz = k + tris[tri].z;
					int gx = 0;
					int gy = 0;
					int gz = 0;
					if (floor(ix) == ix) {
						gx = (int)ix;
					}
					else {
						if (data[(int)floor(ix)][(int)iy][(int)(iz)] == 0) {
							gx = (int)ceil(ix);
						}
						else {
							gx = (int)floor(ix);
						}
					}
					if (floor(iy) == iy) {
						gy = (int)iy;
					}
					else {
						if (data[(int)(ix)][(int)floor(iy)][(int)(iz)] == 0) {
							gy = (int)ceil(iy);
						}
						else {
							gy = (int)floor(iy);
						}
					}
					if (floor(iz) == iz) {
						gz = (int)iz;
					}
					else {
						if (data[(int)(ix)][(int)(iy)][(int)floor(iz)] == 0) {
							gz = (int)ceil(iz);
						}
						else {
							gz = (int)floor(iz);
						}
					}
					tris[tri].r = colorLookup[data[gx][gy][gz]][0];
					tris[tri].g = colorLookup[data[gx][gy][gz]][1];
					tris[tri].b = colorLookup[data[gx][gy][gz]][2];
					tris[tri].ox = i;
					tris[tri].oy = j;
					tris[tri].oz = k;
					l++;
					tri++;
				}
			}
		}
	}
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE - 1; j++) {
			delete[] indices[i][j];
		}
		delete[] indices[i];
	}
	delete[] indices;
	return tris;
}

float** Chunk::trisToFloats(MeshVert* tris, int len) {
	float* mesh = new float[len * 3];
	float* color = new float[len * 3];
	for (int i = 0; i < len; i++) {
		mesh[i * 3] = (tris[i].x + tris[i].ox) * BLOCK_SIZE;
		mesh[i * 3 + 1] = (tris[i].y + tris[i].oy) * BLOCK_SIZE;
		mesh[i * 3 + 2] = (tris[i].z + tris[i].oz) * BLOCK_SIZE;
		color[i * 3] = tris[i].r;
		color[i * 3 + 1] = tris[i].g;
		color[i * 3 + 2] = tris[i].b;
	}
	return new float*[2] { mesh, color };
}