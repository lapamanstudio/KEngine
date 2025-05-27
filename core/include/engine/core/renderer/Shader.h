#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const char* vertSrc, const char* fragSrc);
    ~Shader();

    void   Use()  const { glUseProgram(mID); }
    GLuint Id()   const { return mID; }

    void SetMat4(const char* name, const glm::mat4& m) const;
    void SetVec2(const char* name, const glm::vec2& v) const;
    void SetInt (const char* name, int v)               const;

private:
    GLuint mID = 0;
    static GLuint Compile(GLenum type, const char* src);
};
