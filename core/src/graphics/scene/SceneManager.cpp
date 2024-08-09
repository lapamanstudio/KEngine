#include "graphics/scene/SceneManager.h"

int Property::idCounter = 0;

SceneManager::SceneManager() : camera(std::make_shared<SceneCamera>()) {
    camera.get()->Move(glm::vec2(-10, -10));
}
