#ifndef WORK_SCENE_PANEL_H
#define WORK_SCENE_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/panels/controller/WorkSceneController.h"
#include "window/gui/DockManager.h"

class WorkScenePanel : public IDockablePanel {
public:
    WorkScenePanel(DockManager* dockManager);
    void render(int posX, int posY, int width, int height) override;
private:
    WorkSceneController* workSceneController = nullptr;
    DockManager* dockManager;
};

#endif // WORK_SCENE_PANEL_H
