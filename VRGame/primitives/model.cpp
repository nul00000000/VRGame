#include "model.h"

#include <chrono>

#define time std::chrono::high_resolution_clock::now()
#define micros(x) std::chrono::duration_cast<std::chrono::microseconds>(x).count()
#define millis(x) std::chrono::duration_cast<std::chrono::milliseconds>(x).count()

Model::Model(GLfloat* vertices, GLint vLen, GLfloat r, GLfloat g, GLfloat b, bool bNormals) {
	vertexCount = vLen / 3;
	scale = glm::vec3(1, 1, 1);
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	storeData(0, 3, vLen, vertices, vertexVboID);
	GLfloat* buffer = fillColorArray(r, g, b, vLen / 3, new GLfloat[vLen]);
	storeData(1, 3, vLen, buffer, colorVboID);
	storeData(2, 3, vLen, getNormals(vertices, vLen, buffer, bNormals), normalVboID);
	glBindVertexArray(0);
	delete[] buffer;
}

Model::Model(GLfloat* vertices, GLint vLen, GLfloat* colors, bool bNormals) {
	vertexCount = vLen / 3;
	scale = glm::vec3(1, 1, 1);
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	storeData(0, 3, vLen, vertices, vertexVboID);
	storeData(1, 3, vLen, colors, colorVboID);
	auto a = time;
	GLfloat* buffer = getNormals(vertices, vLen, new GLfloat[vLen], bNormals);
	cout << millis(time - a) << endl;
	storeData(2, 3, vLen, buffer, normalVboID);
	glBindVertexArray(0);
	delete[] buffer;
}

Model::~Model() {
	glDeleteBuffers(1, &vertexVboID);
	glDeleteBuffers(1, &colorVboID);
	glDeleteBuffers(1, &normalVboID);
	glDeleteVertexArrays(1, &vaoID);
}

//please don't pass in vertexCount as the length of buffer i will fucking find whoever does it - Nul 6/10/20
GLfloat* Model::fillColorArray(GLfloat r, GLfloat g, GLfloat b, GLint vertexCount, GLfloat* buffer) {
	for (GLint i = 0; i < vertexCount; i++) {
		buffer[i * 3] = r;
		buffer[i * 3 + 1] = g;
		buffer[i * 3 + 2] = b;
	}
	return buffer;
}

GLfloat* Model::getNormals(GLfloat* vertices, GLint vLen, GLfloat* dest, bool bNormals) {
	for (int i = 0; i < vLen / 9; i++) {
		glm::vec3 point(vertices[i * 9], vertices[i * 9 + 1], vertices[i * 9 + 2]);
		glm::vec3 s1(vertices[i * 9 + 3], vertices[i * 9 + 4], vertices[i * 9 + 5]);
		glm::vec3 s2(vertices[i * 9 + 6], vertices[i * 9 + 7], vertices[i * 9 + 8]);
		s1 -= point;
		s2 -= point;
		glm::vec3 norm = glm::normalize(glm::cross(s1, s2));
		if (bNormals)
			norm = -norm;
		dest[i * 9] = norm.x;
		dest[i * 9 + 1] = norm.y;
		dest[i * 9 + 2] = norm.z;
		dest[i * 9 + 3] = norm.x;
		dest[i * 9 + 4] = norm.y;
		dest[i * 9 + 5] = norm.z;
		dest[i * 9 + 6] = norm.x;
		dest[i * 9 + 7] = norm.y;
		dest[i * 9 + 8] = norm.z;
	}
	return dest;
}

void Model::storeData(GLint index, GLint size, GLint bufferLength, GLfloat* buffer, GLuint &vboID) {
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, (GLint64)bufferLength * 4, buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::render() {
	renderUnsafe();
	glBindVertexArray(0);
}

//transform should theoretically be a finished transform matrix, but I have 
//to work out how scale is going to be applied, for now, just using 
//Matrix4f.scale(Vector), although it may actually need to be applied first
//this actually should be used for efficiency, it just doesn't rebind the vertex array to zero
void Model::renderUnsafe() {
	glBindVertexArray(vaoID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

GLfloat* Model::cubeVertices(GLfloat w, GLfloat h, GLfloat d) {
	return new GLfloat[72]{
		-w / 2, h / 2, d / 2, //top left front
		w / 2, h / 2, d / 2, // top right front
		w / 2, -h / 2, d / 2, //bottom right front
		-w / 2, -h / 2, d / 2, //bottom left front

		w / 2, h / 2, -d / 2, // top right back
		-w / 2, h / 2, -d / 2, //top left back
		-w / 2, -h / 2, -d / 2, //bottom left back
		w / 2, -h / 2, -d / 2, //bottom right back

		-w / 2, h / 2, -d / 2,
		-w / 2, h / 2, d / 2,
		-w / 2, -h / 2, d / 2,
		-w / 2, -h / 2, -d / 2,

		w / 2, h / 2, d / 2,
		w / 2, h / 2, -d / 2,
		w / 2, -h / 2, -d / 2,
		w / 2, -h / 2, d / 2,

		-w / 2, h / 2, -d / 2,
		w / 2, h / 2, -d / 2,
		w / 2, h / 2, d / 2,
		-w / 2, h / 2, d / 2,

		w / 2, -h / 2, -d / 2,
		-w / 2, -h / 2, -d / 2,
		-w / 2, -h / 2, d / 2,
		w / 2, -h / 2, d / 2
	};
}