#include "engine/platform/opengl/GLShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GLShader::GLShader(GLuint programID) : programID(programID) {}

GLShader::~GLShader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

void GLShader::Use() const {
    glUseProgram(programID);
}

void GLShader::Unuse() const {
    glUseProgram(0);
}

GLuint GLShader::GetProgramID() const {
    return programID;
}
