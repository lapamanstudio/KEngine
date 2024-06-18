#ifndef TREE_VIEW_PANEL_H
#define TREE_VIEW_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/DockManager.h"

class TreeViewPanel : public IDockablePanel {
public:
    TreeViewPanel(DockManager* dockManager);
    void render(int posX, int posY, int width, int height) override;
private:
    DockManager* dockManager;
};

#endif // TREE_VIEW_PANEL_H
