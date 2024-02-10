#pragma once

#include <glm/glm.hpp>
#include <string>

using namespace std;

struct Light
{
public:
	Light(string id, glm::vec3 pos, glm::vec3 color, bool active) : id(id), pos(pos), color(color), active(active) {};
	Light(string id, float x, float y, float z, float r, float g, float b, bool active) : id(id), pos(x, y, z), color(r, g, b), active(active) {};
	string id;
	glm::vec3 pos;
	glm::vec3 color;
	bool active;
};

