#ifndef WORK_SCENE_RENDERER_H
#define WORK_SCENE_RENDERER_H

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
    WorkSceneRenderer(bool* debug, int posX, int posY, int width, int height);
    ~WorkSceneRenderer();

    void render(SceneCamera* camera, SceneManager* sceneManager);
    void updateSize(SceneCamera* camera, int newX, int newY, int newWidth, int newHeight);
    GLuint getTexture() const;

    void setSelectionBox(const glm::vec4& box) { selectionBox = box; }
    void clearSelectionBox() { selectionBox = glm::vec4(0.0f); }
private:
    void setupFramebuffer();
    void clearVertexData();

    void renderDebugIfNeeded(SceneCamera* camera);
    void batchRender(SceneCamera* camera, SceneManager* sceneManager);
    
    void setupRuler(SceneCamera* camera);
    void renderRuler(SceneCamera* camera);

    std::shared_ptr<GLHelper> shader;

    std::vector<float> vertexData; // Buffer to store vertex data
    std::vector<float> rulerVertices; // Buffer to store vertex data

    glm::vec4 selectionBox;

    bool* debug;
    int posX, posY, width, height;
    GLuint texture_id, shaderProgram, VAO, VBO, EBO, FBO;
};

#endif // WORK_SCENE_RENDERER_H
