#include <imgui.h>
#include <memory>
#include "window/gui/panels/TreeViewPanel.h"

TreeViewPanel::TreeViewPanel(DockManager* dockManager) : dockManager(dockManager) {}

void TreeViewPanel::render(int posX, int posY, int width, int height) {
    if (dockManager->getWorkSceneController() == nullptr) return;

    std::shared_ptr<SceneManager> sceneManager = dockManager->getWorkSceneController()->getSceneManager();
    if (sceneManager == nullptr) return;

    // TODO: Change to actual scene
    ImGui::SeparatorText("Scene - Default");

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 2.f));
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
    
    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_FramePadding;
    for (auto& object : sceneManager->GetObjects()) {
        if (object == sceneManager->GetActiveObject())
            node_flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::TreeNodeEx((void*)(object.get()), node_flags, (object->GetTypeIcon() + " " + object->GetName()).c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            sceneManager->SetActiveObject(object);
        }
    }
    ImGui::PopStyleVar();
}

TreeViewPanel::~TreeViewPanel() {}
