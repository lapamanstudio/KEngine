#ifndef WORK_SCENE_PANEL_H
#define WORK_SCENE_PANEL_H

#include "window/gui/panels/IDockablePanel.h"

class WorkScenePanel : public IDockablePanel {
public:
    void Render(int posX, int posY, int width, int height) override;
};

#endif // WORK_SCENE_PANEL_H
