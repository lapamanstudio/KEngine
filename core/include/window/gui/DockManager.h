#ifndef DOCK_MANAGER_H
#define DOCK_MANAGER_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/panels/controller/WorkSceneController.h"
#include "window/gui/utils/imGuiUtils.h"
#include "graphics/fonts/Fonts.h"

#include <map>
#include <memory>
#include <string>

#define P_TREE_VIEW_INSPECTOR ICON_FA_TREE " Tree View Inspector"
#define P_PROJECT_FILES ICON_FA_FOLDER_OPEN " Project Files"
#define P_CONSOLE ICON_FA_TERMINAL " Console"
#define P_OBJECT_INSPECTOR ICON_FA_CUBE " Object Inspector"
#define P_WORLD_PROPERTIES ICON_FA_GLOBE " World Properties"
#define P_WORK_SCENE ICON_FA_PENCIL_ALT " Work Scene"

class DockManager {
private:
    std::map<std::string, std::unique_ptr<IDockablePanel>> panels;

    WorkSceneController* workSceneController = nullptr;

public:
    DockManager();
    void RenderPanels();

    WorkSceneController* getWorkSceneController() const { return workSceneController; }
    void setWorkSceneController(WorkSceneController* controller) { workSceneController = controller; }
};

#endif // DOCK_MANAGER_H
