#ifndef GRAPHIC_HELPER_H
#define GRAPHIC_HELPER_H

#include "engine/platform/RenderAPI.h"

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <stdexcept>

class GraphicHelper {
public:
    virtual ~GraphicHelper() = default;

    static GraphicHelper* GetInstance() {
        if (!instance) {
            throw std::runtime_error("GraphicHelper has not been initialized. Call Init(RenderAPI) first.");
        }
        return instance;
    }

    static void Init(RenderAPI api);

    virtual void SetColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) = 0;

    virtual void SetModelMatrix(const glm::mat4& model) = 0;
    virtual void SetViewMatrix(const glm::mat4& view) = 0;
    virtual void SetProjectionMatrix(const glm::mat4& projection) = 0;

    virtual void UseDefaultShader() = 0;

    virtual void FillRect(float x, float y, float width, float height) = 0;
private:
    static GraphicHelper* instance;

    virtual void SetMat4(GLint location, const glm::mat4& mat) = 0;
};

#endif // GRAPHIC_HELPER_H
