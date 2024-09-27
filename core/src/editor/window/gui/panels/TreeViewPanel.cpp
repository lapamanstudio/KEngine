#include "editor/window/gui/panels/TreeViewPanel.h"
#include "editor/graphics/scene/objects/components/CameraComponent.h"

#include <imgui.h>
#include <memory>

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
        if (ImGui::MenuItem("Create Empty")) {
            sceneManager->AddObject(std::make_shared<EmptyObject>(0, 0, 800, 600));
        }
        
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Camera")) {
                std::shared_ptr<EmptyObject> camera = std::make_shared<EmptyObject>(0, 0, 800, 600, "Camera");
                camera->AddComponent(std::make_shared<CameraComponent>(camera));
                sceneManager->AddObject(camera);
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // UTILS - Copy, Paste, Cut
        if (ImGui::MenuItem("Copy", nullptr, false, !sceneManager->GetActiveObjects().empty())) {
            dockManager->getWorkSceneController()->setCopiedObjects(sceneManager->GetActiveObjects());
        }

        if (ImGui::MenuItem("Cut", nullptr, false, !sceneManager->GetActiveObjects().empty())) {
            dockManager->getWorkSceneController()->setCopiedObjects(sceneManager->GetActiveObjects());
            for (const auto& object : sceneManager->GetActiveObjects())
                sceneManager->RemoveObject(object);
            sceneManager->GetActiveObjects().clear();
        }

        if (ImGui::MenuItem("Paste", nullptr, false, !dockManager->getWorkSceneController()->getCopiedObjects().empty())) {
            for (const auto& copiedObject : dockManager->getWorkSceneController()->getCopiedObjects()) {
                auto newObject = copiedObject->Clone();
                sceneManager->AddObject(newObject);
                sceneManager->AddActiveObject(newObject);
            }
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Delete", nullptr, false, !sceneManager->GetActiveObjects().empty())) {
            for (const auto& object : sceneManager->GetActiveObjects())
                sceneManager->RemoveObject(object);
            sceneManager->GetActiveObjects().clear();
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
            ImGui::SetDragDropPayload("DND_GAME_OBJECT", &object, sizeof(std::shared_ptr<EmptyObject>));
            ImGui::Text("%s", (object->GetTypeIcon() + " " + object->GetName()).c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_GAME_OBJECT")) {
                std::shared_ptr<EmptyObject> payloadObject = *(std::shared_ptr<EmptyObject>*)payload->Data;
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
