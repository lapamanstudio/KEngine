#ifndef ENGINE_GLSHADER_HELPER_H
#define ENGINE_GLSHADER_HELPER_H

#include "engine/platform/opengl/GLShader.h"

#include <string>
#include <GL/glew.h>

class GLShaderHelper {
public:
    static GLShader* LoadShaders(const char* vertexShaderSource, const char* fragmentShaderSource);

private:
    static GLuint CompileShader(const GLchar*, GLenum shaderType);
    static bool CheckCompileErrors(GLuint shader, GLenum type);
    static bool CheckLinkErrors(GLuint program);
};

#endif // ENGINE_GLSHADER_HELPER_H
