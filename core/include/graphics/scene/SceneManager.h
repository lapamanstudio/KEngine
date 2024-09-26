#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "core/ProjectConfig.h"
#include "core/utils/FileUtils.h"
#include "graphics/scene/objects/EmptyObject.h"
#include "graphics/scene/SceneCamera.h"
#include "graphics/math/MathUtil.h"

#include <GL/glew.h>
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

    void AddObject(std::shared_ptr<EmptyObject> object) { objects.push_back(object); }

    void RemoveObject(std::shared_ptr<EmptyObject> object) {
        auto it = std::find(objects.begin(), objects.end(), object);
        if (it != objects.end()) {
            objects.erase(it);
        }
    }

    void UpdateAll(float deltaTime) {
        // for (auto& object : objects) {
        //     object->Update(deltaTime);
        // }
    }

    void RenderAll(GLuint shaderProgram) {
        for (auto& object : objects) {
            object->Render(shaderProgram);
        }
    }

    std::optional<glm::vec2> GetMiddlePositionOfActiveObjects() const;

    bool IsActiveObject(std::shared_ptr<EmptyObject> object);
    void ReorderObject(std::shared_ptr<EmptyObject> object, std::shared_ptr<EmptyObject> target);
    
    void RemoveActiveObject(std::shared_ptr<EmptyObject> object);
    void AddActiveObject(std::shared_ptr<EmptyObject> object) { activeObjects.push_back(object); }

    std::unique_ptr<std::vector<std::shared_ptr<EmptyObject>>> GetObjectsInCoords(const glm::vec2& coords) const;
    std::unique_ptr<std::vector<std::shared_ptr<EmptyObject>>> GetObjectsInCoords(const glm::vec4& coords) const;
    const std::vector<std::shared_ptr<EmptyObject>>& GetObjects() const { return objects; }
    std::vector<std::shared_ptr<EmptyObject>>& GetActiveObjects() { return activeObjects; }
    std::shared_ptr<SceneCamera>& getCamera() { return camera; }

    WorkSceneMode GetMode() const { return currentMode; }
    void SetMode(WorkSceneMode mode) { currentMode = mode; }

    void SaveScene();
    void LoadScene();

    void BuildProject();
    void GetAssetsFromScene(const nlohmann::json& j, std::vector<std::string>& assets);

    void RegisterComponents();

    // TODO: Refactor this shit
    void SetMouseInUpperArrow(bool value) { isMouseInUpperArrow = value; }
    void SetMouseInRightArrow(bool value) { isMouseInRightArrow = value; }
    void SetMouseInCenterCircle(bool value) { isMouseInCenterCircle = value; }
    bool IsMouseInUpperArrow() const { return isMouseInUpperArrow; }
    bool IsMouseInRightArrow() const { return isMouseInRightArrow; }
    bool IsMouseInCenterCircle() const { return isMouseInCenterCircle; }
private:
    std::shared_ptr<SceneCamera> camera;

    std::vector<std::shared_ptr<EmptyObject>> objects;
    std::vector<std::shared_ptr<EmptyObject>> activeObjects;

    WorkSceneMode currentMode;
    
    bool isMouseInUpperArrow, isMouseInRightArrow, isMouseInCenterCircle;
};

#endif // SCENE_MANAGER_H
