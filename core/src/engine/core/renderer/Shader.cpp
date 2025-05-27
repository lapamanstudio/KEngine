#include "engine/core/renderer/Shader.h"

#include <iostream>

static void check(GLuint id, bool prog)
{
    GLint ok;
    if (prog) glGetProgramiv(id, GL_LINK_STATUS, &ok);
    else      glGetShaderiv (id, GL_COMPILE_STATUS, &ok);

    if (!ok) {
        char log[512];
        if (prog) glGetProgramInfoLog(id, 512, nullptr, log);
        else      glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << (prog ? "LINK" : "COMPILE") << " ERROR\n" << log << std::endl;
    }
}

GLuint Shader::Compile(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    check(s, false);
    return s;
}

Shader::Shader(const char* vsrc, const char* fsrc)
{
    GLuint vs = Compile(GL_VERTEX_SHADER,   vsrc);
    GLuint fs = Compile(GL_FRAGMENT_SHADER, fsrc);

    mID = glCreateProgram();
    glAttachShader(mID, vs);
    glAttachShader(mID, fs);
    glLinkProgram(mID);
    check(mID, true);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    if (mID) glDeleteProgram(mID);
}

void Shader::SetMat4(const char* n, const glm::mat4& m) const
{ glUniformMatrix4fv(glGetUniformLocation(mID, n), 1, GL_FALSE, &m[0][0]); }

void Shader::SetVec2(const char* n, const glm::vec2& v) const
{ glUniform2fv(glGetUniformLocation(mID, n), 1, &v[0]); }

void Shader::SetInt(const char* n, int v) const
{ glUniform1i(glGetUniformLocation(mID, n), v); }
