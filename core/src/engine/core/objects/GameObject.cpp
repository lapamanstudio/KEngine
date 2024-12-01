#include "engine/core/objects/GameObject.h"
#include "engine/core/objects/components/CameraViewComponent.h"

GameObject::GameObject() {
    transform = new Transform();
    renderer = std::make_unique<Renderer>();
}

GameObject::~GameObject() {
    delete transform;
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
        renderer->Render(transform);
    }
}

void GameObject::AddCamera(CameraViewComponent* camera) {
    this->camera = std::unique_ptr<CameraViewComponent>(camera);
}

void GameObject::AddScript(Script* script) {
    printf("[AddScript] Transform pointer %p\n", transform);
    script->CreateMonoBehaviour((void*)transform);
    scripts.push_back(script);
}

Transform* GameObject::GetTransform() const {
    return transform;
}
