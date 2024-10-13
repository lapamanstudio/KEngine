#include "engine/core/EngineSceneManager.h"

#include "engine/core/renderer/GraphicHelper.h"

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
    GraphicHelper::GetInstance()->FillRect(300.0f, 200.0f, 200.0f, 150.0f);
    
    // for (auto obj : gameObjects) {
    //     obj->Render();
    // }
}
