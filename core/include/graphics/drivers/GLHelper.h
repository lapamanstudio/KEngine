#ifndef GLHELPER_H
#define GLHELPER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class GLHelper {
public:
    GLHelper(const char* vertexPath, const char* fragmentPath);
    ~GLHelper();

    void use();
    void unuse();
    GLuint getProgram() const;

    static void setMat4(const std::string& name, const glm::mat4& mat);
    static void setColor3f(GLfloat r, GLfloat g, GLfloat b);
    static void setColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    static void setModelMatrix(const glm::mat4& model);
    static void setViewMatrix(const glm::mat4& view);
    static void setProjectionMatrix(const glm::mat4& projection);
    static void useShader(GLuint shaderProgram);
    static void unuseShader();
    static GLuint getCurrentShaderProgram();

private:
    GLuint shaderProgram;
    static GLuint currentShaderProgram;

    GLuint loadShader(const char* path, GLenum shaderType);
};

#endif
