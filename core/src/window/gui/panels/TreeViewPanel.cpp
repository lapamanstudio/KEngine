#include <imgui.h>
#include <memory>
#include "window/gui/panels/TreeViewPanel.h"

TreeViewPanel::TreeViewPanel(DockManager* dockManager) : dockManager(dockManager) {}

void TreeViewPanel::render(int posX, int posY, int width, int height) {
    if (dockManager->getWorkSceneController() == nullptr) return;

    std::shared_ptr<SceneManager> sceneManager = dockManager->getWorkSceneController()->getSceneManager();
    if (sceneManager == nullptr) return;

    ImGui::SeparatorText("Scene - Default");

    for (auto& object : sceneManager->GetObjects()) {
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (object == sceneManager->GetActiveObject())
            node_flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::TreeNodeEx((void*)(object.get()), node_flags, object->GetName().c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            sceneManager->SetActiveObject(object);
        }
    }
}
