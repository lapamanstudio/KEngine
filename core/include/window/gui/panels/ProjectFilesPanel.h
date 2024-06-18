#ifndef PROJECT_FILES_PANEL_H
#define PROJECT_FILES_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/DockManager.h"

class ProjectFilesPanel : public IDockablePanel {
public:
    ProjectFilesPanel(DockManager* dockManager);
    void render(int posX, int posY, int width, int height) override;
private:
    DockManager* dockManager;
};

#endif // PROJECT_FILES_PANEL_H
