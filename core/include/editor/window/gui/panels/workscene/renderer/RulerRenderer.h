#ifndef RULER_RENDERER_H
#define RULER_RENDERER_H

#include "editor/graphics/drivers/GLHelper.h"
#include "editor/graphics/scene/SceneCamera.h"
#include "editor/graphics/utils/Colors.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class RulerRenderer {
public:
    RulerRenderer(std::shared_ptr<GLHelper> shader);
    ~RulerRenderer();

    void setupRuler();
    void renderRuler(SceneCamera* camera);
private:
    std::shared_ptr<GLHelper> shader;
    std::vector<float> rulerVertices;

    GLuint VAO, VBO;
};

#endif // RULER_RENDERER_H