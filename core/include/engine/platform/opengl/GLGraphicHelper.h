#ifndef GL_GRAPHIC_HELPER_H
#define GL_GRAPHIC_HELPER_H

#include "engine/core/renderer/GraphicHelper.h"
#include "engine/platform/opengl/GLShader.h"

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class GLGraphicHelper : public GraphicHelper {
public:
    GLGraphicHelper();
    ~GLGraphicHelper();

    void SetColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

    void SetModelMatrix(const glm::mat4& model) override;
    void SetViewMatrix(const glm::mat4& view) override;
    void SetProjectionMatrix(const glm::mat4& projection) override;

    void UseDefaultShader() override;

    void FillRect(float x, float y, float width, float height) override;

private:
    GLuint VAO, VBO;
    GLShader* defaultShader;
    GLuint currentShaderProgram;

    GLint modelLoc;
    GLint viewLoc;
    GLint projLoc;
    GLint colorLoc;

    void SetMat4(GLint location, const glm::mat4& mat);
};

#endif // GL_GRAPHIC_HELPER_H
