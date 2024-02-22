#ifndef WORLD_PROPERTIES_PANEL_H
#define WORLD_PROPERTIES_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/panels/controller/WorkSceneController.h"

class WorldPropertiesPanel : public IDockablePanel {
public:
    void render(int posX, int posY, int width, int height) override;
};

#endif // WORLD_PROPERTIES_PANEL_H
