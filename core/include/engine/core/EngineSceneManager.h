#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "engine/core/objects/GameObject.h"
#include "engine/core/objects/components/CameraViewComponent.h"

#include <vector>

class EngineSceneManager {
public:
    EngineSceneManager();
    ~EngineSceneManager();

    void AddGameObject(GameObject* object);

    void SetMainCamera(CameraViewComponent* camera);
    CameraViewComponent* GetMainCamera() const;

    void Update(float deltaTime);
    void Render();
    
private:
    std::vector<GameObject*> gameObjects;

    CameraViewComponent* mainCamera = nullptr;
};

#endif // SCENE_MANAGER_H
