#ifndef PROJECT_FILES_PANEL_H
#define PROJECT_FILES_PANEL_H

#include "window/gui/panels/IDockablePanel.h"
#include "window/gui/DockManager.h"
#include <string>
#include <unordered_set>
#include <vector>
#include <GL/glew.h> // Include this if you use OpenGL types like GLuint

class ProjectFilesPanel : public IDockablePanel {
public:
    ProjectFilesPanel(DockManager* dockManager);
    ~ProjectFilesPanel();
    void render(int posX, int posY, int width, int height) override;

private:
    struct FileItem {
        std::string name;
        bool isDirectory;
    };

    // Rendering functions
    void renderPathHeader();
    bool renderItem(const FileItem& item);

    // File operations
    void createNewFolder();
    void createNewFile();
    void startRenaming();
    void deleteSelectedItems();
    void saveRenameAndReset();
    void cancelRenaming();
    void selectRange(const std::string& currentItem);
    void MoveItemToPath(const std::string& itemName, const std::string& targetFolder);
    GLuint loadTextureForItem(const FileItem& item);

    // Member variables
    DockManager* dockManager;
    std::string currentDirectory;

    // State flags
    bool isDeleting = false;
    bool isRenaming = false;

    // Renaming
    std::string editingName;
    std::string newName;

    // Items and selection
    std::vector<FileItem> items;
    std::unordered_set<std::string> selectedItems;
    std::string lastSelectedItem;
};

#endif // PROJECT_FILES_PANEL_H
