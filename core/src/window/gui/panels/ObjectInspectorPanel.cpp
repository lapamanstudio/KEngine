#include "window/gui/panels/ObjectInspectorPanel.h"
#include "graphics/scene/objects/components/SpriteRendererComponent.h"
#include "graphics/scene/objects/components/CameraComponent.h"

ObjectInspectorPanel::ObjectInspectorPanel(DockManager* dockManager) : dockManager(dockManager) {}

void ObjectInspectorPanel::render(int posX, int posY, int width, int height) {
    if (dockManager->getWorkSceneController() == nullptr) return;

    std::shared_ptr<SceneManager> sceneManager = dockManager->getWorkSceneController()->getSceneManager();
    if (sceneManager == nullptr) return;

    std::vector<std::shared_ptr<EmptyObject>> activeObjects = dockManager->getWorkSceneController()->getSceneManager()->GetActiveObjects();
    if (activeObjects.size() == 0) {
        ImGui::SeparatorText("No object selected");
        return;
    } else if (activeObjects.size() > 1) {
        ImGui::SeparatorText("Cannot edit multiple objects at once");
        return;
    }
    
    activeObjects.at(0)->RenderProperties();
    ImGui::SeparatorText("");

    // Add Components
    static float buttonWidth = 120.0f;
    static float buttonHeight = 25.0f;
    float buttonPosX = (width - buttonWidth) / 2.0f;

    ImGui::SetCursorPosX((width - buttonWidth) / 2.0f);

    if (ImGui::Button("Add Component", ImVec2(buttonWidth, buttonHeight)))
        ImGui::OpenPopup("ComponentPopup");

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetCursorScreenPos().x + buttonPosX - 7.5f, ImGui::GetCursorScreenPos().y));
    ImGui::SetNextWindowSize(ImVec2(buttonWidth, 0));

    static bool openDuplicatedComponentPopup = false;

    if (ImGui::BeginPopup("ComponentPopup")) {
        if (ImGui::Selectable("Camera")) {
            if (!activeObjects.at(0)->AddComponent(std::make_shared<CameraComponent>(activeObjects.at(0)))) {
                ImGui::CloseCurrentPopup();
                openDuplicatedComponentPopup = true;
            }
        }
        if (ImGui::Selectable("Sprite Renderer")) {
            if (!activeObjects.at(0)->AddComponent(std::make_shared<SpriteRendererComponent>(activeObjects.at(0)))) {
                ImGui::CloseCurrentPopup();
                openDuplicatedComponentPopup = true;
            }
        }
        ImGui::EndPopup();
    }

    if (openDuplicatedComponentPopup) {
        openDuplicatedComponentPopup = false;
        ImGui::OpenPopup("Duplicated component");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Duplicated component", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("This object already has the same component.");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

ObjectInspectorPanel::~ObjectInspectorPanel() {}
