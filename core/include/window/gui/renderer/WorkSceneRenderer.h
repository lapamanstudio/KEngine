#ifndef WORK_SCENE_RENDERER_H
#define WORK_SCENE_RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class WorkSceneRenderer {
public:
    WorkSceneRenderer(int posX, int posY, int width, int height);
    ~WorkSceneRenderer();

    void setupFramebuffer();
    void render();
    void updateSize(int posX, int posY, int width, int height);
    GLuint getTexture() const;

private:
    int posX, posY, width, height;
    GLuint texture_id, shaderProgram;
    GLuint VAO, VBO, EBO, FBO, RBO;  // Include FBO and RBO for framebuffer

    void createShaders();
};

#endif // WORK_SCENE_RENDERER_H
