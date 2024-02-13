#ifndef OBJECT_INSPECTOR_PANEL_H
#define OBJECT_INSPECTOR_PANEL_H

#include "window/gui/panels/IDockablePanel.h"

class ObjectInspectorPanel : public IDockablePanel {
public:
    void Render(int posX, int posY, int width, int height) override;
};

#endif // OBJECT_INSPECTOR_PANEL_H
