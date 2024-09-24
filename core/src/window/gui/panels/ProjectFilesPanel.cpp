#include "window/gui/panels/ProjectFilesPanel.h"
#include "graphics/utils/TextureManager.h"
#include "core/ProjectConfig.h"
#include "core/utils/FileUtils.h"
#include "core/utils/StringUtils.h"

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

namespace fs = std::filesystem;

void ShowInExplorer(const std::string& path) {
    std::string fullPath = path;

    std::replace(fullPath.begin(), fullPath.end(), '/', FileUtils::GetPathSeparator());

#ifdef _WIN32
    ShellExecuteA(nullptr, "open", fullPath.c_str(), nullptr, nullptr, SW_SHOW);
#elif __APPLE__
    std::string command = "open \"" + fullPath + "\"";
    system(command.c_str());
#elif __linux__
    std::string command = "xdg-open \"" + fullPath + "\"";
    system(command.c_str());
#else
    #error "Unsupported platform"
#endif
}


ProjectFilesPanel::ProjectFilesPanel(DockManager* dockManager)
    : dockManager(dockManager), currentDirectory("Assets/") {}

void ProjectFilesPanel::render(int posX, int posY, int width, int height) {
    if (!ProjectConfig::getInstance().isInitialized) {
        return;
    }

    renderPathHeader();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
    ImGui::Separator();

    // Clear items and load directory contents
    items.clear();
    fs::path fullPath = fs::path(ProjectConfig::getInstance().projectDirectory) / currentDirectory;

    if (!fs::exists(fullPath) || !fs::is_directory(fullPath)) {
        currentDirectory = "Assets/";
        return;
    }

    for (const auto& entry : fs::directory_iterator(fullPath)) {
        items.push_back({ entry.path().filename().string(), entry.is_directory() });
    }

    // Sort items: directories first, then files, both alphabetically (case-insensitive)
    std::sort(items.begin(), items.end(), [](const FileItem& a, const FileItem& b) {
        if (a.isDirectory != b.isDirectory)
            return a.isDirectory > b.isDirectory;

        std::string nameA = a.name;
        std::string nameB = b.name;
        std::transform(nameA.begin(), nameA.end(), nameA.begin(), ::tolower);
        std::transform(nameB.begin(), nameB.end(), nameB.begin(), ::tolower);

        return nameA < nameB;
    });

    // Layout calculations
    float panelWidth = ImGui::GetContentRegionAvail().x;
    const ImGuiStyle& style = ImGui::GetStyle();
    float itemSpacingX = style.ItemSpacing.x;
    float iconSize = 64.0f;
    float sideMargin = 8.0f;
    float itemWidth = iconSize + 16.0f + sideMargin * 2;
    int itemsPerRow = std::max(1, static_cast<int>((panelWidth + itemSpacingX) / (itemWidth + itemSpacingX)));
    float adjustedItemSpacingX = itemsPerRow > 1 ? (panelWidth - itemsPerRow * itemWidth) / (itemsPerRow - 1) : 0.0f;

    // Render items
    bool anyItemClicked = false;
    for (size_t i = 0; i < items.size(); ++i) {
        if (i % itemsPerRow != 0)
            ImGui::SameLine(0.0f, adjustedItemSpacingX);

        if (renderItem(items[i]))
            anyItemClicked = true;
    }

    // Context menu
    if (ImGui::BeginPopupContextWindow("##projectFilesPopup", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Create Folder")) {
            createNewFolder();
        }
        if (ImGui::MenuItem("Create File")) {
            createNewFile();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Rename", nullptr, false, selectedItems.size() == 1)) {
            startRenaming();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Delete", nullptr, false, !selectedItems.empty())) {
            isDeleting = true;
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Show in Explorer")) {
            ShowInExplorer(ProjectConfig::getInstance().projectDirectory + FileUtils::GetPathSeparator() + currentDirectory);
        }
        ImGui::EndPopup();
    }

    // Deletion confirmation popup
    if (isDeleting) {
        ImGui::OpenPopup("Delete Confirmation");
        isDeleting = false;
    }

    if (ImGui::BeginPopupModal("Delete Confirmation", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete %zu item(s)?\nThis action cannot be undone.", selectedItems.size());
        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            deleteSelectedItems();
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Click outside items to clear selection
    if (!anyItemClicked && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) {
        selectedItems.clear();
        if (!editingName.empty())
            saveRenameAndReset();
    }

    // Handle renaming cancellation
    if (!editingName.empty() && (ImGui::IsKeyPressed(ImGuiKey_Escape) || (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsItemHovered()))) {
        cancelRenaming();
    }

    // Handle renaming confirmation
    if (!editingName.empty() && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        saveRenameAndReset();
    }
}

void ProjectFilesPanel::saveRenameAndReset() {
    if (!newName.empty()) {
        fs::path oldPath = fs::path(ProjectConfig::getInstance().projectDirectory) / currentDirectory / editingName;
        fs::path newPath = fs::path(ProjectConfig::getInstance().projectDirectory) / currentDirectory / newName;

        if (fs::exists(oldPath) && !fs::exists(newPath)) {
            fs::rename(oldPath, newPath);
        }
    }
    editingName.clear();
    newName.clear();
    isRenaming = false;
}

void ProjectFilesPanel::cancelRenaming() {
    editingName.clear();
    newName.clear();
    isRenaming = false;
}

void ProjectFilesPanel::renderPathHeader() {
    fs::path path(currentDirectory);
    std::vector<fs::path> pathParts;

    for (const auto& part : path) {
        if (!part.empty()) {
            pathParts.push_back(part);
        }
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

    fs::path accumulatedPath;
    for (size_t i = 0; i < pathParts.size(); ++i) {
        accumulatedPath /= pathParts[i];

        ImGui::PushID(i);
        if (ImGui::Button(pathParts[i].string().c_str())) {
            currentDirectory = accumulatedPath.string() + "/";
            selectedItems.clear();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_PAYLOAD")) {
                const char* droppedData = static_cast<const char*>(payload->Data);
                std::istringstream droppedItems(droppedData);
                std::string itemName;
                while (std::getline(droppedItems, itemName)) {
                    if (!itemName.empty()) {
                        MoveItemToPath(itemName, accumulatedPath.string());
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();

        if (i < pathParts.size() - 1) {
            ImGui::SameLine();
            ImGui::Text(">");
            ImGui::SameLine();
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
}

bool ProjectFilesPanel::renderItem(const FileItem& item) {
    ImGui::PushID(item.name.c_str());
    ImGui::BeginGroup();

    // Constants for layout
    const float iconSize = 64.0f;
    const float sideMargin = 8.0f;
    const float padding = 16.0f;
    const float totalWidth = iconSize + padding;
    const float totalHeight = iconSize + ImGui::GetTextLineHeightWithSpacing() + padding;

    const ImVec2 itemSize(totalWidth + sideMargin * 2, totalHeight);

    const bool isSelected = selectedItems.find(item.name) != selectedItems.end();

    const ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    // Invisible button to capture clicks and hover state
    ImGui::InvisibleButton("##itemSelectable", itemSize);

    const bool isItemHovered = ImGui::IsItemHovered();
    const bool isItemClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
    const bool isItemDoubleClicked = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isItemHovered;

    // Track click and drag state
    static std::string clickedItemName;
    static bool isDraggingItem = false;

    // Handle mouse click and drag logic
    if (isItemClicked) {
        clickedItemName = item.name;
        isDraggingItem = false;
    }

    if (clickedItemName == item.name) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            isDraggingItem = true;
            selectedItems.insert(item.name);
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            if (!isDraggingItem) {
                // Process click
                if (ImGui::GetIO().KeyCtrl) {
                    if (isSelected)
                        selectedItems.erase(item.name);
                    else
                        selectedItems.insert(item.name);
                } else if (ImGui::GetIO().KeyShift && !lastSelectedItem.empty()) {
                    selectRange(item.name);
                } else {
                    selectedItems.clear();
                    selectedItems.insert(item.name);
                }
                lastSelectedItem = item.name;
            }
            // Reset click and drag state
            clickedItemName.clear();
            isDraggingItem = false;
        }
    }

    // Handle double-click
    if (isItemDoubleClicked) {
        if (item.isDirectory) {
            currentDirectory += item.name + "/";
            selectedItems.clear();
        } else {
            // openFile(item.name);
        }
    }

    // Handle drag and drop source
    if (clickedItemName == item.name && isDraggingItem && ImGui::BeginDragDropSource()) {
        std::ostringstream payloadStream;
        for (const auto& selectedItem : selectedItems) {
            payloadStream << selectedItem << "\n";
        }
        const std::string payloadData = payloadStream.str();
        ImGui::SetDragDropPayload("ITEM_PAYLOAD", payloadData.c_str(), payloadData.size() + 1);
        ImGui::Text("%s", item.name.c_str());
        ImGui::EndDragDropSource();
    }

    // Handle drag and drop target
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_PAYLOAD")) {
            const char* droppedData = static_cast<const char*>(payload->Data);
            if (item.isDirectory) {
                std::istringstream droppedItems(droppedData);
                std::string itemName;
                while (std::getline(droppedItems, itemName)) {
                    if (itemName == item.name) continue;
                    if (!itemName.empty()) {
                        MoveItemToPath(itemName, currentDirectory + item.name);
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    // Drawing background
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec4 bgColor = isSelected ? ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered) :
                     isItemHovered ? ImGui::GetStyleColorVec4(ImGuiCol_Header) :
                     ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);

    const ImU32 bgCol32 = ImGui::ColorConvertFloat4ToU32(bgColor);
    const float borderRadius = 8.0f;
    const ImRect selectableRect(cursorPos, ImVec2(cursorPos.x + itemSize.x, cursorPos.y + itemSize.y));
    drawList->AddRectFilled(selectableRect.Min, selectableRect.Max, bgCol32, borderRadius);

    // Center the image
    const float imageX = cursorPos.x + sideMargin + (totalWidth - iconSize) / 2.0f;
    const float imageY = cursorPos.y + 8.0f;
    ImGui::SetCursorScreenPos(ImVec2(imageX, imageY));

    GLuint texture = loadTextureForItem(item);

    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), ImVec2(iconSize, iconSize));

    // Draw text
    if (editingName == item.name) {
        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + sideMargin - 4, imageY + iconSize + 1));
        ImGui::SetNextItemWidth(totalWidth + 7);
        ImGui::SetKeyboardFocusHere();
        ImGuiEngined::InputTextStdString("##rename", &newName, ImGuiInputTextFlags_CallbackCharFilter, StringUtils::FilenameFilter);
    } else {
        const std::string displayName = StringUtils::TruncateTextToWidth(item.name, totalWidth);
        const float textWidth = ImGui::CalcTextSize(displayName.c_str()).x;
        const float textX = cursorPos.x + sideMargin + (totalWidth - textWidth) / 2.0f;
        const float textY = imageY + iconSize + 4.0f;
        ImGui::SetCursorScreenPos(ImVec2(textX, textY));
        ImGui::TextWrapped("%s", displayName.c_str());
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", item.name.c_str());
    }

    ImGui::EndGroup();
    ImGui::PopID();

    return isItemClicked;
}

GLuint ProjectFilesPanel::loadTextureForItem(const FileItem& item) {
    static std::unordered_map<std::string, GLuint> iconCache;

    if (item.isDirectory) {
        static GLuint folderTexture = TextureManager::LoadTexture("icons/folder_icon.png");
        return folderTexture;
    } else {
        std::string extension = fs::path(item.name).extension().string();
        if (!extension.empty() && extension[0] == '.') {
            extension = extension.substr(1);
        }
        auto it = iconCache.find(extension);
        if (it != iconCache.end()) {
            return it->second;
        } else {
            std::string iconFilename = "icons/file_icon_" + extension + ".png";
            if (!FileUtils::FileExists(iconFilename)) {
                iconFilename = "icons/file_icon.png";
            }
            GLuint texture = TextureManager::LoadTexture(iconFilename.c_str());
            iconCache[extension] = texture;
            return texture;
        }
    }
}

void ProjectFilesPanel::createNewFolder() {
    std::string newFolderName = "New Folder";
    int folderIndex = 1;

    fs::path fullPath = fs::path(ProjectConfig::getInstance().projectDirectory) / currentDirectory / newFolderName;
    while (fs::exists(fullPath)) {
        newFolderName = "New Folder (" + std::to_string(folderIndex++) + ")";
        fullPath = fs::path(ProjectConfig::getInstance().projectDirectory) / currentDirectory / newFolderName;
    }

    fs::create_directory(fullPath);

    editingName = newFolderName;
    newName = newFolderName;
    isRenaming = true;
}

void ProjectFilesPanel::createNewFile() {
    std::string newFileName = "New File";
    int fileIndex = 1;

    fs::path fullPath = fs::path(ProjectConfig::getInstance().projectDirectory) / currentDirectory / newFileName;
    while (fs::exists(fullPath)) {
        newFileName = "New File (" + std::to_string(fileIndex++) + ")";
        fullPath = fs::path(ProjectConfig::getInstance().projectDirectory) / currentDirectory / newFileName;
    }

    std::ofstream fileOut(fullPath);
    fileOut.close();

    editingName = newFileName;
    newName = newFileName;
    isRenaming = true;
}

void ProjectFilesPanel::startRenaming() {
    editingName = *selectedItems.begin();
    newName = editingName;
    isRenaming = true;
}

void ProjectFilesPanel::deleteSelectedItems() {
    for (const auto& item : selectedItems) {
        fs::path path = fs::path(ProjectConfig::getInstance().projectDirectory) / currentDirectory / item;
        if (fs::exists(path)) {
            if (fs::is_directory(path)) {
                fs::remove_all(path);
            } else {
                fs::remove(path);
            }
        }
    }
    selectedItems.clear();
}

void ProjectFilesPanel::MoveItemToPath(const std::string& itemName, const std::string& targetFolder) {
    fs::path sourcePath = fs::path(ProjectConfig::getInstance().projectDirectory) / currentDirectory / itemName;
    fs::path destinationPath = fs::path(ProjectConfig::getInstance().projectDirectory) / targetFolder / itemName;

    if (fs::exists(sourcePath) && fs::exists(destinationPath.parent_path())) {
        fs::rename(sourcePath, destinationPath);
    }
}

void ProjectFilesPanel::selectRange(const std::string& currentItem) {
    int startIndex = -1;
    int endIndex = -1;
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].name == lastSelectedItem) {
            startIndex = static_cast<int>(i);
        }
        if (items[i].name == currentItem) {
            endIndex = static_cast<int>(i);
        }
    }

    if (startIndex != -1 && endIndex != -1) {
        if (startIndex > endIndex)
            std::swap(startIndex, endIndex);

        for (int i = startIndex; i <= endIndex; ++i) {
            selectedItems.insert(items[i].name);
        }
    }
}

ProjectFilesPanel::~ProjectFilesPanel() {}
