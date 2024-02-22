#ifndef WORK_SCENE_RENDERER_H
#define WORK_SCENE_RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/utils/Camera.h"

class WorkSceneRenderer {
public:
    WorkSceneRenderer(int posX, int posY, int width, int height);
    ~WorkSceneRenderer();

    void render(Camera* camera);
    void updateSize(int posX, int posY, int width, int height);
    GLuint getTexture() const;

private:
    glm::mat4 projection;
    int posX, posY, width, height;
    GLuint texture_id, shaderProgram;
    GLuint VAO, VBO, EBO, FBO, RBO;  // Include FBO and RBO for framebuffer

    void setupFramebuffer();
    void createTriangle();
    void createShaders();
};

#endif // WORK_SCENE_RENDERER_H
