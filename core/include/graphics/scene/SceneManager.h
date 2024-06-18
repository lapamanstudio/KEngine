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
