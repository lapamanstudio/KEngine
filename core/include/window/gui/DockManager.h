#ifndef DOCK_MANAGER_H
#define DOCK_MANAGER_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/panels/controller/WorkSceneController.h"
#include "imgui.h"
#include <map>
#include <memory>
#include <string>

#define P_TREE_VIEW_INSPECTOR "Tree View Inspector"
#define P_PROJECT_FILES "Project Files"
#define P_CONSOLE "Console"
#define P_OBJECT_INSPECTOR "Object Inspector"
#define P_WORLD_PROPERTIES "World Properties"
#define P_WORK_SCENE "Work Scene"

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
