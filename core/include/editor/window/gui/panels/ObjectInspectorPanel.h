#ifndef OBJECT_INSPECTOR_PANEL_H
#define OBJECT_INSPECTOR_PANEL_H

#include "editor/window/gui/panels/IDockablePanel.h"
#include "editor/window/gui/DockManager.h"

class ObjectInspectorPanel : public IDockablePanel {
public:
    ObjectInspectorPanel(DockManager* dockManager);
    ~ObjectInspectorPanel();
    void render(int posX, int posY, int width, int height) override;
private:
    DockManager* dockManager;
};

#endif // OBJECT_INSPECTOR_PANEL_H
