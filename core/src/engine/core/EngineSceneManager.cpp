#include "engine/core/EngineSceneManager.h"
#include "engine/core/renderer/GraphicHelper.h"
#include "engine/core/input/Input.h"

EngineSceneManager::EngineSceneManager() {}

EngineSceneManager::~EngineSceneManager() {
    for (auto obj : gameObjects)
        delete obj;
}

void EngineSceneManager::AddGameObject(GameObject* object) {
    gameObjects.push_back(object);
}

void EngineSceneManager::SetMainCamera(CameraViewComponent* camera) {
    if (mainCamera) {
        mainCamera->isMainCamera = false;
    }
    mainCamera = camera;
    mainCamera->isMainCamera = true;
}

CameraViewComponent* EngineSceneManager::GetMainCamera() const {
    return mainCamera;
}

void EngineSceneManager::Update(float deltaTime) {
    // Update key states
    Input::Update();

    // Update game objects
    for (auto obj : gameObjects) {
        obj->Update(deltaTime);
    }
}

void EngineSceneManager::Render() {
    if (mainCamera) {
        GraphicHelper::GetInstance()->SetProjectionMatrix(mainCamera->GetProjectionMatrix());
        GraphicHelper::GetInstance()->SetViewMatrix(mainCamera->GetViewMatrix());
    }

    GraphicHelper::GetInstance()->UseDefaultShader();

    GraphicHelper::GetInstance()->SetColor4f(1.0f, 0.5f, 0.2f, 1.0f);
    
    for (auto obj : gameObjects) {
        if (obj->GetCamera())
            continue;

        // This is a placeholder for the actual rendering logic
        GraphicHelper::GetInstance()->FillRect(obj->GetTransform()->GetPosition().x, obj->GetTransform()->GetPosition().y, 50, 50);
        obj->Render();
    }
}
