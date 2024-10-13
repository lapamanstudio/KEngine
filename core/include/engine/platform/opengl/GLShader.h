#ifndef ENGINE_GLSHADER_H
#define ENGINE_GLSHADER_H

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

class GLShader {
public:
    GLShader(GLuint programID);
    ~GLShader();

    void Use() const;
    void Unuse() const;

    GLuint GetProgramID() const;
private:
    GLuint programID;
};

#endif // ENGINE_GLSHADER_H
