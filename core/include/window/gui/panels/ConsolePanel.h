#ifndef CONSOLE_PANEL_H
#define CONSOLE_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/DockManager.h"

class ConsolePanel : public IDockablePanel {
public:
    ConsolePanel(DockManager* dockManager);
    void render(int posX, int posY, int width, int height) override;
private:
    DockManager* dockManager;
};

#endif // CONSOLE_PANEL_H
