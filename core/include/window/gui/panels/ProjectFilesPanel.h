#ifndef PROJECT_FILES_PANEL_H
#define PROJECT_FILES_PANEL_H

#include "window/gui/panels/IDockablePanel.h"

class ProjectFilesPanel : public IDockablePanel {
public:
    void Render(int posX, int posY, int width, int height) override;
};

#endif // PROJECT_FILES_PANEL_H
