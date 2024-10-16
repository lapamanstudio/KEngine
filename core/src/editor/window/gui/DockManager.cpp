#include <imgui.h>
#include <imgui_internal.h>

#include "editor/window/gui/DockManager.h"
#include "editor/window/gui/panels/WorkScenePanel.h"
#include "editor/window/gui/panels/TreeViewPanel.h"
#include "editor/window/gui/panels/ConsolePanel.h"
#include "editor/window/gui/panels/ObjectInspectorPanel.h"
#include "editor/window/gui/panels/ProjectFilesPanel.h"
#include "editor/window/gui/panels/WorldPropertiesPanel.h"

DockManager::DockManager() {
    // Initialize panels
    panels[P_WORK_SCENE] = std::make_unique<WorkScenePanel>(this);
    panels[P_TREE_VIEW_INSPECTOR] = std::make_unique<TreeViewPanel>(this);
    panels[P_PROJECT_FILES] = std::make_unique<ProjectFilesPanel>(this);
    panels[P_CONSOLE] = std::make_unique<ConsolePanel>(this);
    panels[P_OBJECT_INSPECTOR] = std::make_unique<ObjectInspectorPanel>(this);
    panels[P_WORLD_PROPERTIES] = std::make_unique<WorldPropertiesPanel>(this);
}

void DockManager::RenderPanels() {
    for (auto& panel : panels) {
        if (ImGui::Begin(panel.first.c_str())) {
            ImVec2 rect = ImGui::GetWindowSize();
            ImVec2 pos = ImGui::GetItemRectMin();
            
            panel.second->render(pos.x, pos.y, rect.x, rect.y);
        }
        ImGui::End();
    }
}
