#include "window/gui/panels/ObjectInspectorPanel.h"

ObjectInspectorPanel::ObjectInspectorPanel(DockManager* dockManager) : dockManager(dockManager) {}

void ObjectInspectorPanel::render(int posX, int posY, int width, int height) {
    if (dockManager->getWorkSceneController() == nullptr) return;

    std::shared_ptr<SceneManager> sceneManager = dockManager->getWorkSceneController()->getSceneManager();
    if (sceneManager == nullptr) return;

    std::shared_ptr<GameObject> object = dockManager->getWorkSceneController()->getSceneManager()->GetActiveObject();
    if (object == nullptr) {
        ImGui::SeparatorText("No object selected");
        return;
    }
    
    object->RenderProperties();
}

ObjectInspectorPanel::~ObjectInspectorPanel() {}