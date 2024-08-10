#ifndef WORK_SCENE_RENDERER_H
#define WORK_SCENE_RENDERER_H

#include "window/gui/panels/workscene/renderer/RulerRenderer.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/scene/SceneCamera.h"
#include "graphics/scene/SceneManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class WorkSceneRenderer {
public:
    WorkSceneRenderer(std::shared_ptr<GLHelper> shader, int posX, int posY, int width, int height);
    ~WorkSceneRenderer();

    void render(SceneCamera* camera, SceneManager* sceneManager);
    void postRender();
    void updateSize(SceneCamera* camera, int newWidth, int newHeight);

    void setSelectionBox(const glm::vec4& box) { selectionBox = box; }
    void clearSelectionBox() { selectionBox = glm::vec4(0.0f); }

    GLuint getTexture() const { return texture_id; }
private:
    void batchRender(SceneCamera* camera, SceneManager* sceneManager);

    std::shared_ptr<RulerRenderer> rulerRenderer;
    std::shared_ptr<GLHelper> shader;

    std::vector<float> vertexData;

    glm::vec4 selectionBox;

    int posX, posY, width, height;
    GLuint texture_id, shaderProgram, VAO, VBO;
};

#endif // WORK_SCENE_RENDERER_H
