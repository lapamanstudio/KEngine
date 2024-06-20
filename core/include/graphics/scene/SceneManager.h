#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "graphics/scene/objects/GameObject.h"

#include <vector>
#include <memory>

class SceneManager {
public:
    void AddObject(std::shared_ptr<GameObject> object) {
        objects.push_back(object);
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

    std::unique_ptr<std::vector<std::shared_ptr<GameObject>>> GetObjectsInCoords(const glm::vec2& coords) const {
        auto objectsInCoords = std::make_unique<std::vector<std::shared_ptr<GameObject>>>();
        for (const auto& object : objects) {
            if (object->IsInCoords(coords)) {
                objectsInCoords->push_back(object);
            }
        }
        return objectsInCoords;
    }
    
    std::unique_ptr<std::vector<std::shared_ptr<GameObject>>> GetObjectsInCoords(const glm::vec4& coords) const {
        auto objectsInCoords = std::make_unique<std::vector<std::shared_ptr<GameObject>>>();

        glm::vec2 pos1(std::min(coords.x, coords.z), std::min(coords.y, coords.w));
        glm::vec2 size1(std::abs(coords.z - coords.x), std::abs(coords.w - coords.y));

        for (const auto& object : objects) {
            glm::vec2 pos2 = object->GetPosition();
            glm::vec2 size2 = object->GetSize();

            if (MathUtil::DoRectsIntersect(pos1, size1, pos2, size2)) {
                objectsInCoords->push_back(object);
            }
        }

        return objectsInCoords;
    }

    const std::vector<std::shared_ptr<GameObject>>& GetObjects() const {
        return objects;
    }

    void SetActiveObject(std::shared_ptr<GameObject> object) {
        activeObject = object;
    }

    std::shared_ptr<GameObject> GetActiveObject() const {
        return activeObject;
    }

private:
    std::vector<std::shared_ptr<GameObject>> objects;
    std::shared_ptr<GameObject> activeObject;
};

#endif // SCENE_MANAGER_H
