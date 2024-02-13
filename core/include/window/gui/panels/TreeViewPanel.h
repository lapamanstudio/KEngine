#ifndef TREE_VIEW_PANEL_H
#define TREE_VIEW_PANEL_H

#include "window/gui/panels/IDockablePanel.h"

class TreeViewPanel : public IDockablePanel {
public:
    void Render(int posX, int posY, int width, int height) override;
};

#endif // TREE_VIEW_PANEL_H
