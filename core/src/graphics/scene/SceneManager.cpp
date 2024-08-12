#include "graphics/scene/SceneManager.h"

int Property::idCounter = 0;

SceneManager::SceneManager() : camera(std::make_shared<SceneCamera>()) {
    camera.get()->Move(glm::vec2(-10, -10));
}

void SceneManager::ReorderObject(std::shared_ptr<GameObject> object, std::shared_ptr<GameObject> target) {
    auto it = std::find(objects.begin(), objects.end(), object);
    if (it != objects.end()) {
        objects.erase(it);
    }
    
    auto targetIt = std::find(objects.begin(), objects.end(), target);
    if (targetIt != objects.end()) {
        objects.insert(targetIt, object);
    } else {
        objects.push_back(object);
    }
}

void SceneManager::RemoveActiveObject(std::shared_ptr<GameObject> object) {
    auto it = std::find(activeObjects.begin(), activeObjects.end(), object);
    if (it != activeObjects.end()) {
        activeObjects.erase(it);
    }
}

bool SceneManager::IsActiveObject(std::shared_ptr<GameObject> object) {
    return std::find(activeObjects.begin(), activeObjects.end(), object) != activeObjects.end();
}

std::unique_ptr<std::vector<std::shared_ptr<GameObject>>> SceneManager::GetObjectsInCoords(const glm::vec4& coords) const {
    auto objectsInCoords = std::make_unique<std::vector<std::shared_ptr<GameObject>>>();

    glm::vec2 pos1(std::min(coords.x, coords.z), std::min(coords.y, coords.w));
    glm::vec2 size1(std::abs(coords.z - coords.x), std::abs(coords.w - coords.y));

    for (const auto& object : objects) {
        glm::vec2 pos2 = object->GetPosition();
        glm::vec2 size2 = object->GetSize();
        float rotation = object->GetRotation();

        if (MathUtil::DoRectsIntersect(pos1, size1, pos2, size2, rotation)) {
            objectsInCoords->push_back(object);
        }
    }

    return objectsInCoords;
}

std::unique_ptr<std::vector<std::shared_ptr<GameObject>>> SceneManager::GetObjectsInCoords(const glm::vec2& coords) const {
    auto objectsInCoords = std::make_unique<std::vector<std::shared_ptr<GameObject>>>();
    for (const auto& object : objects) {
        if (object->IsInCoords(coords)) {
            objectsInCoords->push_back(object);
        }
    }
    return objectsInCoords;
}

std::optional<glm::vec2> SceneManager::GetMiddlePositionOfActiveObjects() const {
    if (activeObjects.empty()) return std::nullopt;

    glm::vec2 middle(0.0f, 0.0f);

    for (const auto& object : activeObjects) {
        middle += object->GetPosition() + object->GetSize() / 2.0f;
    }

    middle /= static_cast<float>(activeObjects.size());

    return middle;
}
