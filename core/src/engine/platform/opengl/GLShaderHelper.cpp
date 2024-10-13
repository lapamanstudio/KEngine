#include "engine/platform/opengl/GLShaderHelper.h"

#include <iostream>

GLShader* GLShaderHelper::LoadShaders(const char* vertexShaderSource, const char* fragmentShaderSource) {
    GLuint vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    if (!vertexShader || !fragmentShader) {
        return nullptr;
    }

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    if (!CheckLinkErrors(programID)) {
        glDeleteProgram(programID);
        return nullptr;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return new GLShader(programID);
}

GLuint GLShaderHelper::CompileShader(const GLchar* source, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    if (!CheckCompileErrors(shader, shaderType)) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool GLShaderHelper::CheckCompileErrors(GLuint shader, GLenum type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Error: Shader Compilation Error of type " 
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "\n"
                  << infoLog << std::endl;
        return false;
    }
    return true;
}

bool GLShaderHelper::CheckLinkErrors(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "Error: Program Linking Error\n" << infoLog << std::endl;
        return false;
    }
    return true;
}
