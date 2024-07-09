#include <imgui.h>
#include <memory>
#include "window/gui/panels/TreeViewPanel.h"
#include "graphics/scene/objects/Camera.h"

TreeViewPanel::TreeViewPanel(DockManager* dockManager) : dockManager(dockManager) {}

void TreeViewPanel::render(int posX, int posY, int width, int height) {
    if (dockManager->getWorkSceneController() == nullptr) return;

    std::shared_ptr<SceneManager> sceneManager = dockManager->getWorkSceneController()->getSceneManager();
    if (sceneManager == nullptr) return;

    // Popup menu
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.1);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Camera")) {
                sceneManager->AddObject(std::make_shared<Camera>(0, 0, 800, 600));
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(4);

    // TODO: Change the name to the actual scene name (when implemented)
    ImGui::SeparatorText("Scene - Default");

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 2.f));
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding;
    bool itemClicked = false;

    for (auto& object : sceneManager->GetObjects()) {
        ImGuiTreeNodeFlags currentNodeFlags = node_flags;

        for (auto& activeObject : sceneManager->GetActiveObjects())
            if (object == activeObject)
                currentNodeFlags |= ImGuiTreeNodeFlags_Selected;

        bool nodeOpen = ImGui::TreeNodeEx((void*)(object.get()), currentNodeFlags, (object->GetTypeIcon() + " " + object->GetName()).c_str());

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            itemClicked = true;
            if (ImGui::GetIO().KeyShift) {
                // Shift-click logic
                auto& activeObjects = sceneManager->GetActiveObjects();
                auto it = std::find(activeObjects.begin(), activeObjects.end(), object);
                if (it != activeObjects.end()) {
                    activeObjects.erase(it);
                } else {
                    sceneManager->AddActiveObject(object);
                }
            } else {
                sceneManager->GetActiveObjects().clear();
                sceneManager->AddActiveObject(object);
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("DND_GAME_OBJECT", &object, sizeof(std::shared_ptr<GameObject>));
            ImGui::Text("%s", (object->GetTypeIcon() + " " + object->GetName()).c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_GAME_OBJECT")) {
                std::shared_ptr<GameObject> payloadObject = *(std::shared_ptr<GameObject>*)payload->Data;
                if (payloadObject != object) {
                    sceneManager->ReorderObject(payloadObject, object);
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (nodeOpen) ImGui::TreePop();
    }

    ImGui::PopStyleVar();

    // Check if the background of the TreeViewPanel was clicked
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !itemClicked)
        sceneManager->GetActiveObjects().clear();
}

TreeViewPanel::~TreeViewPanel() {}
