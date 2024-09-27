#include "editor/window/gui/panels/WorldPropertiesPanel.h"

WorldPropertiesPanel::WorldPropertiesPanel(DockManager* dockManager) : dockManager(dockManager) {
    properties = Properties();
}

void ZoomFilter(float& value) {
    if (value < 0.1f) value = 0.1f;
    if (value > 10.0f) value = 10.0f;
}

void WorldPropertiesPanel::render(int posX, int posY, int width, int height) {
    if (!dockManager->getWorkSceneController() || !dockManager->getWorkSceneController()->getSceneManager()->getCamera().get())
        return;

    if (!initialized) {
        glm::vec2& position = const_cast<glm::vec2&>(dockManager->getWorkSceneController()->getSceneManager()->getCamera().get()->GetPosition());
        float& zoom = dockManager->getWorkSceneController()->getSceneManager()->getCamera().get()->GetZoom();

        properties.AddProperty(std::make_shared<Vec2FloatProperty>("Position", &position.x, &position.y, 1.0f));
        properties.AddProperty(std::make_shared<FloatProperty>("Zoom", &zoom, 0.01f, ZoomFilter));
        
        initialized = true;
    }

    ImGui::SeparatorText("Scene Camera properties");
    properties.Render();
}

WorldPropertiesPanel::~WorldPropertiesPanel() {}
