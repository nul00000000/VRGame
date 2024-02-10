#include "shader.h"

BasicShader::BasicShader(GLint screenWidth, GLint screenHeight) : Shader((GLchar*)"C:/Users/Admin/source/repos/VRGame/VRGame/shaders/basic") {//remember to change these when you actually release it
	GLint p;

	string a((GLchar*)"C:/Users/Admin/source/repos/VRGame/VRGame/shaders/basic");
	const char* c_str = readFile((char*)(a + ".geom").c_str());
	geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

	glShaderSource(geometryShaderID, 1, &c_str, NULL);
	glCompileShader(geometryShaderID);

	glGetShaderiv(geometryShaderID, GL_COMPILE_STATUS, &p);
	if (!p) {
		fprintf(stderr, "Geometry shader didn't work go debug it dumbass\n");
	}
	
	delete[] c_str;

	glAttachShader(programID, geometryShaderID);

	bindAttributes();

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &p);
	if(!p) {
	    fprintf(stderr, "The program won't link, what the fuck did you do\n");
	}
	getUniforms();
	
	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &p);
	if (!p) {
	    fprintf(stderr, "The program won't validate, what the fuck did you do\n");
	}
}

BasicShader::BasicShader() : Shader((GLchar*)"C:/Users/Admin/source/repos/VRGame/VRGame/shaders/basic") {
	GLint p;

	string a((GLchar*)"C:/Users/Admin/source/repos/VRGame/VRGame/shaders/basic");
	const char* c_str = readFile((char*)(a + ".geom").c_str());
	geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

	glShaderSource(geometryShaderID, 1, &c_str, NULL);
	glCompileShader(geometryShaderID);

	glGetShaderiv(geometryShaderID, GL_COMPILE_STATUS, &p);
	if (!p) {
		fprintf(stderr, "Geometry shader didn't work go debug it dumbass\n");
	}

	delete[] c_str;

	glAttachShader(programID, geometryShaderID);

	bindAttributes();

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &p);
	if (p) {
		fprintf(stderr, "The program won't link, what the fuck did you do");
	}

	getUniforms();

	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &p);
	if (p) {
		fprintf(stderr, "The program won't validate, what the fuck did you do");
	}
}

BasicShader::~BasicShader() {
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);//Don't know if these first 5 lines are inherited or something but
	glDeleteProgram(programID);      //I've put them here anyway just in case
}

void BasicShader::bindAttributes() {
	bindAttribute(0, (GLchar*)"position");
	bindAttribute(1, (GLchar*)"color");
	bindAttribute(2, (GLchar*)"normal");
}

void BasicShader::getUniforms() {
	transLoc = getUniform((GLchar*)"transform\0");
	camLoc = getUniform((GLchar*)"camera\0");
	projLoc = getUniform((GLchar*)"projection\0");
	lightLoc = getUniform((GLchar*)"light\0");
	lightColorLoc = getUniform((GLchar*)"lightColor\0");
	shinynessLoc = getUniform((GLchar*)"specStrength\0");
	skyLoc = getUniform((GLchar*)"skyAmbience\0");
	lightColorLoc = getUniform((GLchar*)"lightCount\0");
}

void BasicShader::loadProjection(glm::mat4 proj) {
	loadMatrixUniform(4, glm::value_ptr(proj), projLoc);
}

void BasicShader::loadTransform(GLfloat* mTransform, GLfloat shinyness) {
	loadMatrixUniform(4, mTransform, transLoc);
	loadVectorUniform(1, &shinyness, shinynessLoc);
}

void BasicShader::loadCamera(GLfloat* mCamera) {
	loadMatrixUniform(4, mCamera, camLoc);
}

void BasicShader::loadTransform(glm::mat4 mTransform, GLfloat shinyness) {
	loadMatrixUniform(4, glm::value_ptr(mTransform), transLoc);
	loadVectorUniform(1, &shinyness, shinynessLoc);
}

void BasicShader::loadCamera(glm::mat4 mCamera) {
	loadMatrixUniform(4, glm::value_ptr(mCamera), camLoc);
}

void BasicShader::loadSkyLight(glm::vec3 color) {
	loadVectorUniform(3, glm::value_ptr(color), skyLoc);
}

void BasicShader::addLight(Light light) {
	lights.push_back(light);
}

void BasicShader::removeLight(string id) {
	int r;
	for (int i = 0; i < lights.size(); i++) {
		if (lights[i].id == id) {
			r = i;
		}
	}
	lights.erase(lights.begin() + r);
}

void BasicShader::loadLights() {
	int lightC = (lights.size() > MAX_LIGHTS ? MAX_LIGHTS : lights.size());
	glUniform1iv(lightCountLoc, 1, &lightC);
	GLfloat* thing = new GLfloat[MAX_LIGHTS * 3];
	for (int i = 0; i < lightC; i++) {
		thing[i * 3] = lights[i].pos.x;
		thing[i * 3 + 1] = lights[i].pos.y;
		thing[i * 3 + 2] = lights[i].pos.z;
	}
	for (int i = lightC; i < MAX_LIGHTS; i++) {
		thing[i * 3] = 0;
		thing[i * 3 + 1] = 0;
		thing[i * 3 + 2] = 0;
	}
	glUniform3fv(lightLoc, MAX_LIGHTS, thing);
	for (int i = 0; i < lightC; i++) {
		thing[i * 3] = lights[i].color.x;
		thing[i * 3 + 1] = lights[i].color.y;
		thing[i * 3 + 2] = lights[i].color.z;
	}
	for (int i = lightC; i < MAX_LIGHTS; i++) {
		thing[i * 3] = 0;
		thing[i * 3 + 1] = 0;
		thing[i * 3 + 2] = 0;
	}
	glUniform3fv(lightColorLoc, MAX_LIGHTS, thing);
	delete[] thing;
}