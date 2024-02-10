#include "entity.h"

Entity::Entity(Model model) : model(model) {
	pos = glm::vec3(0.0f);
	rot = glm::fquat();
	scale = glm::vec3(1.0f);
}

Entity::~Entity() {
	
}

glm::mat4 Entity::getTransformationMatrix() {
	return glm::translate(glm::toMat4(rot) * (glm::scale(glm::scale(glm::mat4(1.0f), model.scale), scale)), pos);
}