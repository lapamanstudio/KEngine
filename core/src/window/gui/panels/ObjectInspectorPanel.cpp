#include "window/gui/panels/ObjectInspectorPanel.h"

ObjectInspectorPanel::ObjectInspectorPanel(DockManager* dockManager) : dockManager(dockManager) {}

void ObjectInspectorPanel::render(int posX, int posY, int width, int height) {
    if (dockManager->getWorkSceneController() == nullptr) return;

    std::shared_ptr<SceneManager> sceneManager = dockManager->getWorkSceneController()->getSceneManager();
    if (sceneManager == nullptr) return;

    std::vector<std::shared_ptr<GameObject>> activeObjects = dockManager->getWorkSceneController()->getSceneManager()->GetActiveObjects();
    if (activeObjects.size() == 0) {
        ImGui::SeparatorText("No object selected");
        return;
    } else if (activeObjects.size() > 1) {
        ImGui::SeparatorText("Cannot edit multiple objects at once");
        return;
    }
    
    activeObjects.at(0)->RenderProperties();
}

ObjectInspectorPanel::~ObjectInspectorPanel() {}