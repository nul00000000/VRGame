#pragma once

#ifndef GLFW_KEY_LAST
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../light.h"
#include <vector>
#include <string>

#define MAX_LIGHTS 10

using namespace std;

class Shader {
public:
	Shader(GLchar* shaderPath);
	~Shader();
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLuint programID;
	void use();
protected:
	virtual void bindAttributes() = 0;
	virtual void getUniforms() = 0;
	void bindAttribute(GLint index, GLchar* name);
	GLint getUniform(GLchar* name);
	void loadMatrixUniform(GLint len, GLfloat* value, GLint location);
	void loadVectorUniform(GLint len, GLfloat* value, GLint location);
	char* readFile(GLchar* filePath);
};

class BasicShader : public Shader {
public:
	const GLfloat FOV = 90.0f;
	const GLfloat NEAR_PLANE = 0.001f;
	const GLfloat FAR_PLANE = 16.0f;
	GLuint geometryShaderID;
	GLint transLoc, camLoc, projLoc, lightLoc, lightColorLoc, shinynessLoc, skyLoc, lightCountLoc;
	BasicShader(GLint screenWidth, GLint screenHeight);
	BasicShader();
	~BasicShader();
	void loadProjection(glm::mat4 proj);
	void loadTransform(GLfloat* mTransform, GLfloat shinyness);
	void loadCamera(GLfloat* mCamera);
	void loadTransform(glm::mat4 mTransform, GLfloat shinyness);
	void loadCamera(glm::mat4 mCamera);
	void loadSkyLight(glm::vec3 color);
	void addLight(Light light);
	void removeLight(string id);
	void loadLights();
	vector<Light> lights;
protected:
	void bindAttributes();
	void getUniforms();
};