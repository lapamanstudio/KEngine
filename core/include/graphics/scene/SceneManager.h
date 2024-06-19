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
        for (auto& object : objects) {
            if (object->IsInCoords(coords)) {
                objectsInCoords->push_back(object);
            }
        }
        return objectsInCoords;
    }
    
    std::unique_ptr<std::vector<std::shared_ptr<GameObject>>> GetObjectsInCoords(const glm::vec4& coords) const {
        auto objectsInCoords = std::make_unique<std::vector<std::shared_ptr<GameObject>>>();

        float minX = std::min(coords.x, coords.z);
        float maxX = std::max(coords.x, coords.z);
        float minY = std::min(coords.y, coords.w);
        float maxY = std::max(coords.y, coords.w);

        for (const auto& object : objects) {
            glm::vec2 position = object->GetPosition();
            glm::vec2 size = object->GetSize();

            float objMinX = position.x;
            float objMaxX = position.x + size.x;
            float objMinY = position.y;
            float objMaxY = position.y + size.y;

            bool intersects = (minX <= objMaxX && maxX >= objMinX && minY <= objMaxY && maxY >= objMinY);

            if (intersects) {
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
