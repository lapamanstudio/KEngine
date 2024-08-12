#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "graphics/scene/objects/GameObject.h"
#include "graphics/scene/SceneCamera.h"

#include <vector>
#include <memory>
#include <optional>

enum WorkSceneMode {
    FREE_CAMERA_MODE,
    TRANSLATION_MODE
};

class SceneManager {
public:
    SceneManager();

    void AddObject(std::shared_ptr<GameObject> object) { objects.push_back(object); }

    void RemoveObject(std::shared_ptr<GameObject> object) {
        auto it = std::find(objects.begin(), objects.end(), object);
        if (it != objects.end()) {
            objects.erase(it);
        }
    }

    void UpdateAll(float deltaTime) {
        for (auto& object : objects) {
            object->Update(deltaTime);
        }
    }

    void RenderAll(GLuint shaderProgram) {
        for (auto& object : objects) {
            object->Render(shaderProgram);
        }
    }

    std::optional<glm::vec2> GetMiddlePositionOfActiveObjects() const;

    bool IsActiveObject(std::shared_ptr<GameObject> object);
    void ReorderObject(std::shared_ptr<GameObject> object, std::shared_ptr<GameObject> target);
    
    void RemoveActiveObject(std::shared_ptr<GameObject> object);
    void AddActiveObject(std::shared_ptr<GameObject> object) { activeObjects.push_back(object); }

    std::unique_ptr<std::vector<std::shared_ptr<GameObject>>> GetObjectsInCoords(const glm::vec2& coords) const;
    std::unique_ptr<std::vector<std::shared_ptr<GameObject>>> GetObjectsInCoords(const glm::vec4& coords) const;
    const std::vector<std::shared_ptr<GameObject>>& GetObjects() const { return objects; }
    std::vector<std::shared_ptr<GameObject>>& GetActiveObjects() { return activeObjects; }
    std::shared_ptr<SceneCamera>& getCamera() { return camera; }

    WorkSceneMode GetMode() const { return currentMode; }
    void SetMode(WorkSceneMode mode) { currentMode = mode; }

    void SetMouseInUpperArrow(bool value) { isMouseInUpperArrow = value; }
    void SetMouseInRightArrow(bool value) { isMouseInRightArrow = value; }
    void SetMouseInCenterCircle(bool value) { isMouseInCenterCircle = value; }
    bool IsMouseInUpperArrow() const { return isMouseInUpperArrow; }
    bool IsMouseInRightArrow() const { return isMouseInRightArrow; }
    bool IsMouseInCenterCircle() const { return isMouseInCenterCircle; }
private:
    std::shared_ptr<SceneCamera> camera;

    std::vector<std::shared_ptr<GameObject>> objects;
    std::vector<std::shared_ptr<GameObject>> activeObjects;

    WorkSceneMode currentMode;
    
    bool isMouseInUpperArrow, isMouseInRightArrow, isMouseInCenterCircle;
};

#endif // SCENE_MANAGER_H
