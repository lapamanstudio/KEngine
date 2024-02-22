#ifndef WORK_SCENE_CONTROLLER_H
#define WORK_SCENE_CONTROLLER_H

#include "window/gui/renderer/WorkSceneRenderer.h"
#include "graphics/utils/Camera.h"

class WorkSceneController {
public:
    WorkSceneController(int x, int y, int w, int h);
    ~WorkSceneController();

    void update(GLFWwindow* window);
    void render(int x, int y, int w, int h);
    void MoveCamera(float x, float y);
    GLuint getTexture();
private:
    WorkSceneRenderer* workSceneRenderer;
    Camera* camera;

    // Dragging
    bool isMouseDragging = false;
    double lastMouseX, lastMouseY;
};

#endif // WORK_SCENE_CONTROLLER_H
