#include "GLHelper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

GLuint GLHelper::currentShaderProgram = 0;

GLHelper::GLHelper(const char* vertexShaderSource, const char* fragmentShaderSource) {
    GLuint vertexShader = loadShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GLHelper::~GLHelper() {
    glDeleteProgram(shaderProgram);
}

void GLHelper::use() {
    glUseProgram(shaderProgram);
    currentShaderProgram = shaderProgram;
}

void GLHelper::unuse() {
    glUseProgram(0);
    currentShaderProgram = 0;
}

GLuint GLHelper::getProgram() const {
    return shaderProgram;
}

GLuint GLHelper::loadShader(const GLchar* shader_code, GLenum shader_type) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_code, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

void GLHelper::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(currentShaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void GLHelper::setColor3f(GLfloat r, GLfloat g, GLfloat b) {
    GLint colorLoc = glGetUniformLocation(currentShaderProgram, "vertexColor");
    glUniform4f(colorLoc, r, g, b, 1.0f);
}

void GLHelper::setColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    GLint colorLoc = glGetUniformLocation(currentShaderProgram, "vertexColor");
    glUniform4f(colorLoc, r, g, b, a);
}

void GLHelper::setModelMatrix(const glm::mat4& model) {
    setMat4("model", model);
}

void GLHelper::setViewMatrix(const glm::mat4& view) {
    setMat4("view", view);
}

void GLHelper::setProjectionMatrix(const glm::mat4& projection) {
    setMat4("projection", projection);
}

void GLHelper::useShader(GLuint shaderProgram) {
    glUseProgram(shaderProgram);
    currentShaderProgram = shaderProgram;
}

void GLHelper::unuseShader() {
    glUseProgram(0);
    currentShaderProgram = 0;
}

GLuint GLHelper::getCurrentShaderProgram() {
    return currentShaderProgram;
}
