#pragma once

#ifndef GLFW_KEY_LAST
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#include <iostream>

using namespace std;

class FBO {
public:
	FBO(GLuint width, GLuint height);
	~FBO();
	GLuint handle;
	GLuint width;
	GLuint height;
	GLuint colorTextureHandle;
	GLuint depthTextureHandle;
	GLuint resolveHandle;
	GLuint rbHandle;
};