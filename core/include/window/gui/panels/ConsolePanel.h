#ifndef CONSOLE_PANEL_H
#define CONSOLE_PANEL_H

#include "window/gui/panels/IDockablePanel.h"

class ConsolePanel : public IDockablePanel {
public:
    void render(int posX, int posY, int width, int height) override;
};

#endif // CONSOLE_PANEL_H
