#ifndef WORK_SCENE_RENDERER_H
#define WORK_SCENE_RENDERER_H

#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/SceneCamera.h"
#include "graphics/scene/SceneManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class WorkSceneRenderer {
public:
    WorkSceneRenderer(int posX, int posY, int width, int height);
    ~WorkSceneRenderer();

    void render(SceneCamera* camera, SceneManager* sceneManager);
    void updateSize(int newX, int newY, int newWidth, int newHeight);
    GLuint getTexture() const;

private:
    void setupFramebuffer();
    void batchRender(SceneCamera* camera, SceneManager* sceneManager);
    void clearVertexData();

    std::shared_ptr<GLHelper> shader;

    std::vector<float> vertexData; // Buffer to store vertex data

    int posX, posY, width, height;
    GLuint texture_id, shaderProgram, VAO, VBO, EBO, FBO;
    glm::mat4 projection;
};

#endif // WORK_SCENE_RENDERER_H
