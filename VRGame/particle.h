#pragma once

#include "primitives/model.h"
#include "glm/glm.hpp"
#include "shaders/shader.h"

class Particle
{
public:
	Particle();
	void render(BasicShader* shader);
	glm::vec3 pos;
	glm::quat rot;
	float size;
	static void initModel();
private:
	static Model* createModel(int edgeSubdivisions, float radius);
	static Model* sphere;
};
