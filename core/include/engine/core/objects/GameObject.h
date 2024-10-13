#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "engine/core/objects/components/Transform.h"
#include "engine/core/objects/components/Renderer.h"
#include "engine/core/objects/components/Script.h"
#include "engine/core/objects/components/CameraViewComponent.h"

#include <memory>
#include <vector>

class GameObject {
public:
    GameObject();
    ~GameObject();

    void Update(float deltaTime);
    void Render();

    void AddScript(Script* script);
    void AddCamera(CameraViewComponent* camera);

    Transform* GetTransform() { return transform.get(); }
    Renderer* GetRenderer() { return renderer.get(); }
    CameraViewComponent* GetCamera() { return camera.get(); }

private:
    std::unique_ptr<Transform> transform;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<CameraViewComponent> camera;

    std::vector<Script*> scripts;
};

#endif // GAMEOBJECT_H
