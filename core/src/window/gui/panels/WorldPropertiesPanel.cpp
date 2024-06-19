#include "window/gui/panels/WorldPropertiesPanel.h"

WorldPropertiesPanel::WorldPropertiesPanel(DockManager* dockManager) : dockManager(dockManager) {}

void WorldPropertiesPanel::render(int posX, int posY, int width, int height) {
    glm::vec2& position = const_cast<glm::vec2&>(dockManager->getWorkSceneController()->getCamera()->GetPosition());
    float zoom = dockManager->getWorkSceneController()->getCamera()->GetZoom();

    ImGui::SeparatorText("Camera properties");
    ImGui::DragFloat("X##CameraX", &position.x, 1.0f);
    ImGui::DragFloat("Y##CameraY", &position.y, 1.0f);
    if (ImGui::DragFloat("Zoom##CameraZoom", &zoom, 0.01f, 0.1f, 2.0f)) {
        if (zoom < 0.1f)
            zoom = 0.1f;
        if (zoom > 2.0f)
            zoom = 2.0f;
        dockManager->getWorkSceneController()->getCamera()->SetZoom(zoom);
    }
}
