#pragma once

#ifndef GLFW_KEY_LAST
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#include "model.h"
#include "../shaders/shader.h"

class Scene {
	virtual void addModel(Model model) = 0;
	virtual void update(GLfloat delta) = 0;
	virtual void render(BasicShader shader) = 0;
};