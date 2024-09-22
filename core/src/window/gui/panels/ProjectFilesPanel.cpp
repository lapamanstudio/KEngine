#include "window/gui/panels/ProjectFilesPanel.h"
#include "graphics/utils/TextureManager.h"
#include "core/ProjectConfig.h"
#include "core/FileUtils.h"
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

ProjectFilesPanel::ProjectFilesPanel(DockManager* dockManager) : dockManager(dockManager), currentDirectory("Assets/") {}

void ProjectFilesPanel::render(int posX, int posY, int width, int height) {
    if (!ProjectConfig::getInstance().isInitialized) {
        return;
    }
    
    renderPathHeader();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
    ImGui::Separator();

    std::vector<fs::directory_entry> directories, files;
    for (const auto& entry : fs::directory_iterator(ProjectConfig::getInstance().projectDirectory + getPathSeparator() + currentDirectory)) {
        if (entry.is_directory()) {
            directories.push_back(entry);
        } else {
            files.push_back(entry);
        }
    }

    // Render the directories (with folder icons) as clickable items
    for (const auto& dir : directories) {
        renderFolder(dir.path().filename().string());
    }

    // Render the files (for now, assuming file icons are not needed, but can be added similarly)
    // for (const auto& file : files) {
    //     ImGui::Text("%s", file.path().filename().string().c_str());
    // }
}

void ProjectFilesPanel::renderPathHeader() {
    std::vector<std::string> pathParts;

    for (const auto& part : fs::path(currentDirectory)) {
        if (!part.empty()) {
            pathParts.push_back(part.string());
        }
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

    for (size_t i = 0; i < pathParts.size(); ++i) {
        if (ImGuiEngined::ButtonEx(pathParts[i].c_str(), ImVec2(0, 0), ImGuiButtonFlags_None, ImVec4(1.0f, 1.0f, 1.0f, 1.0f))) {
            currentDirectory = "";
            for (size_t j = 0; j <= i; ++j) {
                currentDirectory += pathParts[j] + "/";
            }
        }

        if (i < pathParts.size() - 1) {
            ImGui::SameLine();
            ImGui::Text(">");
            ImGui::SameLine();
        }
    }
    
    ImGui::PopStyleColor(3);
}

void ProjectFilesPanel::renderFolder(const std::string& folderName) {
    ImGui::PushID(folderName.c_str());
    if (ImGui::ImageButton("#", (void*)(intptr_t)TextureManager::LoadTexture("icons/folder_icon.png"), ImVec2(48, 48))) {
        currentDirectory += folderName + "/";
    }
    ImGui::PopID();

    ImGui::TextWrapped("%s", folderName.c_str());
}

ProjectFilesPanel::~ProjectFilesPanel() {}
