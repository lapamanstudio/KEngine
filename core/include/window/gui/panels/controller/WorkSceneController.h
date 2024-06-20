#ifndef WORK_SCENE_CONTROLLER_H
#define WORK_SCENE_CONTROLLER_H

#include "window/gui/renderer/WorkSceneRenderer.h"
#include "graphics/scene/SceneCamera.h"
#include "graphics/scene/SceneManager.h"

class WorkSceneController {
public:
    WorkSceneController(int x, int y, int w, int h);
    ~WorkSceneController();

    void update(GLFWwindow* window, float mouseWheel, float deltaTime);
    void render(int x, int y, int w, int h);
    void MoveCamera(float x, float y);
    void ModifyZoom(float zoom);
    
    std::shared_ptr<SceneCamera> getCamera();
    GLuint getTexture();
    bool isDebug() const;

    std::shared_ptr<SceneManager> getSceneManager() {
        return sceneManager;
    }
    
private:
    void processKeyboardInput(GLFWwindow* window, float deltaTime);
    void processMouseInput(GLFWwindow* window, float mouseWheel, float deltaTime);

    std::shared_ptr<WorkSceneRenderer> workSceneRenderer;
    std::shared_ptr<SceneCamera> camera;
    std::shared_ptr<SceneManager> sceneManager;

    // Dragging
    bool isDebugging;
    bool isMouseDragging;
    bool isMouseSelecting;
    bool isMouseSelectBlocked;
    double lastMouseX, lastMouseY;

    // Container information
    int posX;
    int posY;
    int width;
    int height;

    // Keys
    bool isF3Pressed;

    // Cursors
    GLFWcursor* crossCursor;
};

#endif // WORK_SCENE_CONTROLLER_H
