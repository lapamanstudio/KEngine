#include "engine/core/objects/GameObject.h"
#include "engine/core/objects/components/CameraViewComponent.h"

GameObject::GameObject() {
    transform = std::make_unique<Transform>();
    renderer = std::make_unique<Renderer>();
}

GameObject::~GameObject() {
    for (auto script : scripts) {
        delete script;
    }
}

void GameObject::Update(float deltaTime) {
    for (auto script : scripts) {
        script->Update(deltaTime);
    }
}

void GameObject::Render() {
    if (renderer) {
        renderer->Render(transform.get());
    }
}

void GameObject::AddScript(Script* script) {
    scripts.push_back(script);
}

void GameObject::AddCamera(CameraViewComponent* camera) {
    this->camera = std::unique_ptr<CameraViewComponent>(camera);
}
