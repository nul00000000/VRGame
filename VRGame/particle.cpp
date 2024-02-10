#include "particle.h"
#include <glm/gtx/quaternion.hpp>
#include <random>

Particle::Particle() : pos(0), rot(0, 0, 0, 1), size(1.0f) {

}

void Particle::render(BasicShader* shader) {
	shader->loadTransform(glm::translate(pos) * glm::toMat4(rot) * glm::scale(glm::vec3(size)), 0);
	sphere->render();
}

//edgeSubdivisions is the amount of squares in a row of a cube face essentially, but amountOfSquaresInRowOfCubeFace is quite a variable name
Model* Particle::createModel(int edgeSubdivisions, float radius) {
	int tris = edgeSubdivisions * edgeSubdivisions * 12;
	int a = edgeSubdivisions * edgeSubdivisions * 18;
	float* mesh = new float[tris * 9];
	for (int i = 0; i < tris * 9; i++) {
		mesh[i] = 0;
	}
	float unit = 1.0f / edgeSubdivisions;
	for (int j = 0; j < edgeSubdivisions; j++) {
		for (int k = 0; k < edgeSubdivisions; k++) {
			mesh[(j * edgeSubdivisions + k) * 18] = j * unit + unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 1] = k * unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 2] = -0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 3] = j * unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 4] = k * unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 5] = -0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 6] = j * unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 7] = k * unit + unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 8] = -0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 9] = j * unit + unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 10] = k * unit + unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 11] = -0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 12] = j * unit + unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 13] = k * unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 14] = -0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 15] = j * unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 16] = k * unit + unit - 0.5f;
			mesh[(j * edgeSubdivisions + k) * 18 + 17] = -0.5f;
		}
	}
	for (int j = 0; j < edgeSubdivisions; j++) {
		for (int k = 0; k < edgeSubdivisions; k++) {
			mesh[a + (j * edgeSubdivisions + k) * 18] = j * unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 1] = k * unit + unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 2] = 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 3] = j * unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 4] = k * unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 5] = 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 6] = j * unit + unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 7] = k * unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 8] = 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 9] = j * unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 10] = k * unit + unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 11] = 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 12] = j * unit + unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 13] = k * unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 14] = 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 15] = j * unit + unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 16] = k * unit + unit - 0.5f;
			mesh[a + (j * edgeSubdivisions + k) * 18 + 17] = 0.5f;
		}
	}
	for (int j = 0; j < edgeSubdivisions; j++) {
		for (int k = 0; k < edgeSubdivisions; k++) {
			mesh[2 * a + (j * edgeSubdivisions + k) * 18] = 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 1] = j * unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 2] = k * unit + unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 3] = 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 4] = j * unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 5] = k * unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 6] = 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 7] = j * unit + unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 8] = k * unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 9] = 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 10] = j * unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 11] = k * unit + unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 12] = 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 13] = j * unit + unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 14] = k * unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 15] = 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 16] = j * unit + unit - 0.5f;
			mesh[2 * a + (j * edgeSubdivisions + k) * 18 + 17] = k * unit + unit - 0.5f;
		}
	}
	for (int j = 0; j < edgeSubdivisions; j++) {
		for (int k = 0; k < edgeSubdivisions; k++) {
			mesh[3 * a + (j * edgeSubdivisions + k) * 18] = -0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 1] = j * unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 2] = k * unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 3] = -0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 4] = j * unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 5] = k * unit + unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 6] = -0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 7] = j * unit + unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 8] = k * unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 9] = -0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 10] = j * unit + unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 11] = k * unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 12] = -0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 13] = j * unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 14] = k * unit + unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 15] = -0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 16] = j * unit + unit - 0.5f;
			mesh[3 * a + (j * edgeSubdivisions + k) * 18 + 17] = k * unit + unit - 0.5f;
		}
	}
	for (int j = 0; j < edgeSubdivisions; j++) {
		for (int k = 0; k < edgeSubdivisions; k++) {
			mesh[4 * a + (j * edgeSubdivisions + k) * 18] = j * unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 1] = -0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 2] = k * unit + unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 3] = j * unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 4] = -0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 5] = k * unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 6] = j * unit + unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 7] = -0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 8] = k * unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 9] = j * unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 10] = -0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 11] = k * unit + unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 12] = j * unit + unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 13] = -0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 14] = k * unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 15] = j * unit + unit - 0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 16] = -0.5f;
			mesh[4 * a + (j * edgeSubdivisions + k) * 18 + 17] = k * unit + unit - 0.5f;
		}
	}
	for (int j = 0; j < edgeSubdivisions; j++) {
		for (int k = 0; k < edgeSubdivisions; k++) {
			mesh[5 * a + (j * edgeSubdivisions + k) * 18] = j * unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 1] = 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 2] = k * unit + unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 3] = j * unit + unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 4] = 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 5] = k * unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 6] = j * unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 7] = 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 8] = k * unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 9] = j * unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 10] = 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 11] = k * unit + unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 12] = j * unit + unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 13] = 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 14] = k * unit + unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 15] = j * unit + unit - 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 16] = 0.5f;
			mesh[5 * a + (j * edgeSubdivisions + k) * 18 + 17] = k * unit - 0.5f;
		}
	}
	for (int i = 0; i < tris * 3; i++) {
		float x = mesh[i * 3];
		float y = mesh[i * 3 + 1];
		float z = mesh[i * 3 + 2];
		float len = sqrt(x * x + y * y + z * z) / radius;
		mesh[i * 3] /= len;
		mesh[i * 3 + 1] /= len;
		mesh[i * 3 + 2] /= len;
	}
	return new Model(mesh, tris * 9, 1.0, 0.0, 0.0, false);
}

Model* Particle::sphere = nullptr;

void Particle::initModel() {
	sphere = createModel(4, 0.05f);
}