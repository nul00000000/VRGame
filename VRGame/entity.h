#pragma once

#include <iostream>
#include "glm/glm.hpp"

class Entity
{
public:
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 vel;
	void applyForce(glm::vec3 force);
	void update(float delta);
};