#ifndef PROJECT_FILES_PANEL_H
#define PROJECT_FILES_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/DockManager.h"

class ProjectFilesPanel : public IDockablePanel {
public:
    ProjectFilesPanel(DockManager* dockManager);
    ~ProjectFilesPanel();
    void render(int posX, int posY, int width, int height) override;
private:
    void renderPathHeader();
    void renderFolder(const std::string& folderName);
    
    DockManager* dockManager;

    std::string currentDirectory;
};

#endif // PROJECT_FILES_PANEL_H
