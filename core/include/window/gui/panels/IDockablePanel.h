#ifndef DOCKABLE_PANEL_H
#define DOCKABLE_PANEL_H

class IDockablePanel {
public:
    virtual ~IDockablePanel() {}
    virtual void Render(int posX, int posY, int width, int height) = 0;
};

#endif // DOCKABLE_PANEL_H
