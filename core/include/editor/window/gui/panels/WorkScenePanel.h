#ifndef WORK_SCENE_PANEL_H
#define WORK_SCENE_PANEL_H

#include "editor/window/gui/panels/IDockablePanel.h"
#include "editor/window/gui/panels/workscene/controller/WorkSceneController.h"
#include "editor/window/gui/DockManager.h"

class WorkScenePanel : public IDockablePanel {
public:
    WorkScenePanel(DockManager* dockManager);
    ~WorkScenePanel();
    void render(int posX, int posY, int width, int height) override;
private:
    WorkSceneController* workSceneController = nullptr;
    DockManager* dockManager;
};

#endif // WORK_SCENE_PANEL_H
