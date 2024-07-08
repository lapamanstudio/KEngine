#ifndef WORLD_PROPERTIES_PANEL_H
#define WORLD_PROPERTIES_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/panels/controller/WorkSceneController.h"
#include "window/gui/DockManager.h"
#include "graphics/scene/property/ObjectProperty.h"

class WorldPropertiesPanel : public IDockablePanel {
public:
    WorldPropertiesPanel(DockManager* dockManager);
    void render(int posX, int posY, int width, int height) override;
private:
    DockManager* dockManager;

    Properties properties;
    bool initialized = false;
};

#endif // WORLD_PROPERTIES_PANEL_H
