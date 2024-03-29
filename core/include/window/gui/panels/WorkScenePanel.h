#ifndef WORK_SCENE_PANEL_H
#define WORK_SCENE_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/panels/controller/WorkSceneController.h"

class WorkScenePanel : public IDockablePanel {
public:
    void render(int posX, int posY, int width, int height) override;
private:
    WorkSceneController* workSceneController;
};

#endif // WORK_SCENE_PANEL_H
