#ifndef WORK_SCENE_CONTROLLER_H
#define WORK_SCENE_CONTROLLER_H

#include "graphics/scene/SceneCamera.h"
#include "graphics/scene/SceneManager.h"
#include "window/gui/panels/workscene/renderer/WorkSceneRenderer.h"

class WorkSceneUI;

enum WorkSceneMode {
    FREE_CAMERA_MODE,
    TRANSLATION_MODE
};

class WorkSceneController {
public:
    WorkSceneController(int x, int y, int w, int h);
    ~WorkSceneController();

    void update(GLFWwindow* window, float mouseWheel, float deltaTime, bool mouseInPanel);
    void render(int x, int y, int w, int h);
    
    GLuint getTexture();
    std::shared_ptr<SceneManager> getSceneManager() { return sceneManager; }
    
    const std::vector<std::shared_ptr<GameObject>>& getCopiedObjects() const { return copiedObjects; }
    void setCopiedObjects(const std::vector<std::shared_ptr<GameObject>>& copiedObjects) { this->copiedObjects = copiedObjects; }

    WorkSceneMode getMode() const { return currentMode; }
    void setMode(WorkSceneMode mode);

private:
    void processKeyboardInput(GLFWwindow* window, float deltaTime);
    void processMouseInput(GLFWwindow* window, float mouseWheel, float deltaTime, bool mouseInPanel);

    void updateActiveButton();

    std::shared_ptr<GLHelper> shader;
    std::shared_ptr<WorkSceneRenderer> workSceneRenderer;
    std::shared_ptr<SceneManager> sceneManager;
    std::shared_ptr<WorkSceneUI> workSceneUI;

    std::vector<std::shared_ptr<GameObject>> copiedObjects;

    WorkSceneMode currentMode;

    // Dragging
    bool isMouseDragging;
    bool isMouseSelecting;
    bool isMouseSelectBlocked;
    double lastMouseX, lastMouseY;

    int lastSelectedObjectIndex = -1;

    // Container information
    int posX;
    int posY;
    int width;
    int height;

    // Cursors
    GLFWcursor* crossCursor;
};

#endif // WORK_SCENE_CONTROLLER_H
