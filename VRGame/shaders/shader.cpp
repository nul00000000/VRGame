#include "shader.h"

Shader::Shader(GLchar* shaderPath) {
    string a(shaderPath);
    const char* c_str = readFile((char*)(a + ".vert").c_str());

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    programID = glCreateProgram();

	glShaderSource(vertexShaderID, 1, &c_str, NULL);
    glCompileShader(vertexShaderID);
    GLint p;
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &p);
    if (!p) {
        fprintf(stderr, "Vertex shader didn't work go debug it dumbass\n");
    }

    delete[] c_str;

    const char* c_str2 = readFile((char*)(a + ".frag").c_str());

    glShaderSource(fragmentShaderID, 1, &c_str2, NULL);
    glCompileShader(fragmentShaderID);
    p = 0;
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &p);
    if (!p) {
        fprintf(stderr, "Fragment shader didn't work go debug it dumbass\n");
    }

    delete[] c_str2;
    
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    //bindAttributes(); haha just kidding gotta do it in the children

    //glLinkProgram(programID);
    //glGetProgramiv(programID, GL_LINK_STATUS, &p);
    //if(p) {
    //    fprintf(stderr, "The program won't link, what the fuck did you do");
    //}
    //
    //getUniforms();
    //
    //glValidateProgram(programID);
    //glGetProgramiv(programID, GL_VALIDATE_STATUS, &p);
    //if (p) {
    //    fprintf(stderr, "The program won't validate, what the fuck did you do");
    //}
}

Shader::~Shader() {
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    glDeleteProgram(programID);
}

//this creates a NEW POINTER you MUST delete[] it dumbass future adam
char* Shader::readFile(GLchar* filePath) {
    std::ifstream is(filePath, std::ifstream::binary);
    if (is) {
        is.seekg(0, is.end);
        GLint length = (int) is.tellg();
        is.seekg(0, is.beg);

        char* buffer = new char[(GLint64) length + 1];

        is.read(buffer, length);

        is.close();

        buffer[length] = NULL;

        return buffer;
    }
    fprintf(stderr, "Shader \"%s\" could not be found or read.", filePath);
    return nullptr;
}

void Shader::use() {
    glUseProgram(programID);
}

void Shader::bindAttribute(GLint index, GLchar* name) {
    glBindAttribLocation(programID, index, name);
}

GLint Shader::getUniform(GLchar* name) {
    return glGetUniformLocation(programID, name);
}

void Shader::loadMatrixUniform(GLint len, GLfloat* value, GLint location) {
    switch (len) {
    case 2:
        glUniformMatrix2fv(location, 1, false, value);
        break;
    case 3:
        glUniformMatrix3fv(location, 1, false, value);
        break;
    case 4:
        glUniformMatrix4fv(location, 1, false, value);
        break;
    default:
        break;
    }
}

void Shader::loadVectorUniform(GLint len, GLfloat* value, GLint location) {
    switch (len) {
    case 1:
        glUniform1fv(location, 1, value);
        break;
    case 2:
        glUniform2fv(location, 1, value);
        break;
    case 3:
        glUniform3fv(location, 1, value);
        break;
    case 4:
        glUniform4fv(location, 1, value);
        break;
    default:
        break;
    }
}