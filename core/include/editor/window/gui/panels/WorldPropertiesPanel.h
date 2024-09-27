#ifndef WORLD_PROPERTIES_PANEL_H
#define WORLD_PROPERTIES_PANEL_H

#include "editor/window/gui/panels/IDockablePanel.h"
#include "editor/window/gui/panels/workscene/controller/WorkSceneController.h"
#include "editor/window/gui/DockManager.h"
#include "editor/graphics/scene/property/ObjectProperty.h"

class WorldPropertiesPanel : public IDockablePanel {
public:
    WorldPropertiesPanel(DockManager* dockManager);
    ~WorldPropertiesPanel();
    void render(int posX, int posY, int width, int height) override;
private:
    DockManager* dockManager;

    Properties properties;
    bool initialized = false;
};

#endif // WORLD_PROPERTIES_PANEL_H
