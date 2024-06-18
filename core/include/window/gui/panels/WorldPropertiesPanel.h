#ifndef WORLD_PROPERTIES_PANEL_H
#define WORLD_PROPERTIES_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/panels/controller/WorkSceneController.h"
#include "window/gui/DockManager.h"

class WorldPropertiesPanel : public IDockablePanel {
public:
    WorldPropertiesPanel(DockManager* dockManager);
    void render(int posX, int posY, int width, int height) override;
private:
    DockManager* dockManager;
};

#endif // WORLD_PROPERTIES_PANEL_H
