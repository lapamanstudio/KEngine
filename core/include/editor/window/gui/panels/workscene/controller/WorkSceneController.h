#ifndef WORK_SCENE_CONTROLLER_H
#define WORK_SCENE_CONTROLLER_H

#include "editor/graphics/scene/SceneCamera.h"
#include "editor/graphics/scene/SceneManager.h"
#include "editor/window/gui/panels/workscene/renderer/WorkSceneRenderer.h"

class WorkSceneUI;

class WorkSceneController {
public:
    WorkSceneController(int x, int y, int w, int h);
    ~WorkSceneController();

    void update(GLFWwindow* window, float mouseWheel, float deltaTime, bool mouseInPanel);
    void render(int x, int y, int w, int h);
    
    GLuint getTexture();
    std::shared_ptr<SceneManager> getSceneManager() { return sceneManager; }
    
    const std::vector<std::shared_ptr<EmptyObject>>& getCopiedObjects() const { return copiedObjects; }
    void setCopiedObjects(const std::vector<std::shared_ptr<EmptyObject>>& copiedObjects) { this->copiedObjects = copiedObjects; }
private:
    void processKeyboardInput(GLFWwindow* window, float deltaTime);
    void processMouseInput(GLFWwindow* window, float mouseWheel, float deltaTime, bool mouseInPanel);

    void updateActiveButton(WorkSceneMode mode);
    void setMode(WorkSceneMode mode);

    std::shared_ptr<GLHelper> shader;
    std::shared_ptr<WorkSceneRenderer> workSceneRenderer;
    std::shared_ptr<SceneManager> sceneManager;
    std::shared_ptr<WorkSceneUI> workSceneUI;

    std::vector<std::shared_ptr<EmptyObject>> copiedObjects;

    // Dragging
    bool isMouseMovingObjectsFromCenter, isMouseMovingObjectsFromUpper, isMouseMovingObjectsFromRight;
    bool isMouseDragging;
    bool isMouseSelecting;
    bool isMouseSelectBlocked;
    double lastMouseX, lastMouseY;

    int lastSelectedObjectIndex = -1;

    int mouseDirectionWhenMovingX = 0;
    int mouseDirectionWhenMovingY = 0;

    // Container information
    int posX;
    int posY;
    int width;
    int height;

    // Cursors
    GLFWcursor* crossCursor;
};

#endif // WORK_SCENE_CONTROLLER_H
