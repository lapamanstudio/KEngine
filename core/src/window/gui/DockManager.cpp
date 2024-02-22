#include <imgui.h>
#include <imgui_internal.h>

#include "window/gui/DockManager.h"
#include "window/gui/panels/WorkScenePanel.h"
#include "window/gui/panels/TreeViewPanel.h"
#include "window/gui/panels/ConsolePanel.h"
#include "window/gui/panels/ObjectInspectorPanel.h"
#include "window/gui/panels/ProjectFilesPanel.h"
#include "window/gui/panels/WorldPropertiesPanel.h"

DockManager::DockManager() {
    // Initialize panels
    panels[P_WORK_SCENE] = std::make_unique<WorkScenePanel>();
    panels[P_TREE_VIEW_INSPECTOR] = std::make_unique<TreeViewPanel>();
    panels[P_PROJECT_FILES] = std::make_unique<ProjectFilesPanel>();
    panels[P_CONSOLE] = std::make_unique<ConsolePanel>();
    panels[P_OBJECT_INSPECTOR] = std::make_unique<ObjectInspectorPanel>();
    panels[P_WORLD_PROPERTIES] = std::make_unique<WorldPropertiesPanel>();
}

void DockManager::RenderPanels() {
    for (auto& panel : panels) {
        if (ImGui::Begin(panel.first.c_str())) {
            ImVec2 rect = ImGui::GetWindowSize();
            ImVec2 pos = ImGui::GetWindowPos();
            panel.second->render(pos.x, pos.y, rect.x, rect.y);
        }
        ImGui::End();
    }
}
