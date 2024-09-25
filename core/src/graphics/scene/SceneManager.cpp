#include "graphics/scene/SceneManager.h"
#include "graphics/scene/property/ObjectProperty.h"
#include "graphics/scene/objects/components/CameraComponent.h"
#include "graphics/scene/objects/components/SpriteRendererComponent.h"

int Property::idCounter = 0;

SceneManager::SceneManager() : camera(std::make_shared<SceneCamera>()) {
    camera.get()->Move(glm::vec2(-10, -10));
    RegisterComponents();
}

void SceneManager::ReorderObject(std::shared_ptr<EmptyObject> object, std::shared_ptr<EmptyObject> target) {
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

void SceneManager::SaveScene() {
    nlohmann::json sceneJson = nlohmann::json::array();

    for (auto& object : objects) {
        sceneJson.push_back(object->Serialize());
    }

    std::ofstream file(ProjectConfig::getInstance().projectDirectory / "scene.json");

    if (file.is_open()) {
        file << sceneJson.dump(2);
        file.close();
        std::cout << "Scene saved to scene.json" << std::endl;
    } else {
        std::cerr << "Error: Unable to open file for saving the scene." << std::endl;
    }
}

void SceneManager::LoadScene() {
    std::ifstream file(ProjectConfig::getInstance().projectDirectory / "scene.json");

    if (file.is_open()) {
        nlohmann::json sceneJson;
        file >> sceneJson;
        file.close();

        for (auto& objectJson : sceneJson) {
            auto object = EmptyObject::Deserialize(objectJson);
            objects.push_back(object);
        }

        std::cout << "Scene loaded from scene.json" << std::endl;
    } else {
        std::cerr << "Error: Unable to open file for loading the scene." << std::endl;
    }
}

void SceneManager::BuildProject() {
    // Extract resources being used on the scene!
    std::vector<std::string> resources;

    // TODO: This is only getting assets from the first scene (scene.json by default), in the future must contain multiple scenes!
    

    // Build resources
}

void SceneManager::RegisterComponents() {
    ObjectComponentFactory::RegisterComponent<CameraComponent>("Camera");
    ObjectComponentFactory::RegisterComponent<SpriteRendererComponent>("Sprite Renderer");
}

void SceneManager::RemoveActiveObject(std::shared_ptr<EmptyObject> object) {
    auto it = std::find(activeObjects.begin(), activeObjects.end(), object);
    if (it != activeObjects.end()) {
        activeObjects.erase(it);
    }
}

bool SceneManager::IsActiveObject(std::shared_ptr<EmptyObject> object) {
    return std::find(activeObjects.begin(), activeObjects.end(), object) != activeObjects.end();
}

std::unique_ptr<std::vector<std::shared_ptr<EmptyObject>>> SceneManager::GetObjectsInCoords(const glm::vec4& coords) const {
    auto objectsInCoords = std::make_unique<std::vector<std::shared_ptr<EmptyObject>>>();

    glm::vec2 pos1(std::min(coords.x, coords.z), std::min(coords.y, coords.w));
    glm::vec2 size1(std::abs(coords.z - coords.x), std::abs(coords.w - coords.y));

    for (const auto& object : objects) {
        glm::vec2 pos2 = object->GetPosition();
        glm::vec2 size2 = object->GetSize() * object->GetScale();
        float rotation = object->GetRotation();

        if (MathUtil::DoRectsIntersect(pos1, size1, pos2, size2, rotation)) {
            objectsInCoords->push_back(object);
        }
    }

    return objectsInCoords;
}

std::unique_ptr<std::vector<std::shared_ptr<EmptyObject>>> SceneManager::GetObjectsInCoords(const glm::vec2& coords) const {
    auto objectsInCoords = std::make_unique<std::vector<std::shared_ptr<EmptyObject>>>();
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
