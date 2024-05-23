#ifndef WORK_SCENE_CONTROLLER_H
#define WORK_SCENE_CONTROLLER_H

#include "window/gui/renderer/WorkSceneRenderer.h"
#include "graphics/utils/SceneCamera.h"
#include "graphics/scene/SceneManager.h"

class WorkSceneController {
public:
    WorkSceneController(int x, int y, int w, int h);
    ~WorkSceneController();

    void update(GLFWwindow* window, float mouseWheel);
    void render(int x, int y, int w, int h);
    void MoveCamera(float x, float y);
    void ModifyZoom(float zoom);

    GLuint getTexture();
    bool isDebug() const;
private:
    std::shared_ptr<WorkSceneRenderer> workSceneRenderer;
    std::shared_ptr<SceneCamera> camera;
    std::shared_ptr<SceneManager> sceneManager;

    // Dragging
    bool isDebugging;
    bool isMouseDragging;
    double lastMouseX, lastMouseY;

    // Container information
    int posX;
    int posY;
    int width;
    int height;

    // Keys
    bool isF3Pressed;
};

#endif // WORK_SCENE_CONTROLLER_H
