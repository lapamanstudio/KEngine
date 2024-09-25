#include <cstdlib>
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "core/ProjectConfig.h"
#include "core/utils/FileUtils.h"
#include "graphics/fonts/Fonts.h"
#include "graphics/utils/TextureManager.h"
#include "window/Window.h"
#include "window/gui/DockManager.h"
#include "window/gui/utils/imGuiUtils.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
#endif

DockManager dockManager;
GLuint splash_texture;

void OpenURL(const char* url) {
#if defined(_WIN32)
    ShellExecute(0, 0, url, 0, 0, SW_SHOW);
#elif defined(__APPLE__)
    system(("open " + std::string(url)).c_str());
#else
    system(("xdg-open " + std::string(url)).c_str());
#endif
}

void initialize_window(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Enable docking
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    ImFontConfig font_config = ImFontConfig();

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config; 
    icons_config.MergeMode = true; 
    icons_config.PixelSnapH = true; 
    icons_config.GlyphMinAdvanceX = 16;
    float iconFontSize = 18 * 2.0f / 3.0f; // 13px is the font size
    io.Fonts->AddFontFromFileTTF(FileUtils::GetDataFilePath("fonts\\Roboto-Regular.ttf").c_str(), 16, &font_config, io.Fonts->GetGlyphRangesDefault());
    io.Fonts->AddFontFromMemoryTTF(font_awesome_ttf, font_awesome_ttf_len, iconFontSize, &icons_config, icons_ranges);
    io.IniFilename = NULL; // Disable INI file saving

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 7.5f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);  // Dark background
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // Dark title bar background
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Slightly brighter title bar when active
    style.Colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Dark separator color

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // Dark child background
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Slightly brighter child background when hovered
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Slightly brighter child background when active

    style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f); // Dark header background
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Hovered header background
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Active header background

    style.Colors[ImGuiCol_Separator] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f); // Slightly brighter separator color
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Hovered separator color
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Active separator color

    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.8f, 0.8f, 0.8f, 0.10f); // Grip color
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.60f); // Hovered grip color

    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f); // Default button color
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f); // Hovered button color
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // Active button color

    style.Colors[ImGuiCol_Tab] = ImLerp(style.Colors[ImGuiCol_Header], style.Colors[ImGuiCol_TitleBgActive], 0.80f); // Dark tab color
    style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_HeaderHovered]; // Hovered tab color
    style.Colors[ImGuiCol_TabActive] = ImLerp(style.Colors[ImGuiCol_HeaderActive], style.Colors[ImGuiCol_TitleBgActive], 0.60f); // Active tab color
    style.Colors[ImGuiCol_TabUnfocused] = ImLerp(style.Colors[ImGuiCol_Tab], style.Colors[ImGuiCol_TitleBg], 0.80f); // Unfocused tab color
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImLerp(style.Colors[ImGuiCol_TabActive], style.Colors[ImGuiCol_TitleBg], 0.40f); // Unfocused active tab color
    style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0, 0, 0, 0);

    style.Colors[ImGuiCol_DockingPreview] = style.Colors[ImGuiCol_Header]; // Docking preview color

    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Drag and drop target color

    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
}

void initDockLayout() {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    const ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoSplit;

    // Clear any previous layout
    ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
    ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace); // Add a new DockSpace node

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

    // Split the dockspace to create left, right, and bottom docks, leaving the center as the main area
    auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15f, nullptr, &dockspace_id);
    auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.17f, nullptr, &dockspace_id);
    auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);

    // Dock windows into their respective places
    ImGui::DockBuilderDockWindow(P_TREE_VIEW_INSPECTOR, dock_id_left);
    ImGui::DockBuilderDockWindow(P_PROJECT_FILES, dock_id_down);
    ImGui::DockBuilderDockWindow(P_CONSOLE, dock_id_down);
    ImGui::DockBuilderDockWindow(P_OBJECT_INSPECTOR, dock_id_right);
    ImGui::DockBuilderDockWindow(P_WORLD_PROPERTIES, dock_id_right);

    // The central area is automatically determined by the docking system
    ImGui::DockBuilderDockWindow(P_WORK_SCENE, dockspace_id);

    ImGui::DockBuilderFinish(dockspace_id);

    splash_texture = TextureManager::LoadTexture("splash.png");
}

void render_window() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    glViewport(0, 0, static_cast<int>(viewport->Size.x), static_cast<int>(viewport->Size.y));
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Menu Bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Project")) {
            // Project menu items
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            // Edit menu items
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            // Help menu items
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Setup Docking Environment
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton;

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3); // Popping all style vars

    // DockSpace
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        
        static bool first_time = true;
        if (first_time) {
            first_time = false;
            initDockLayout();
        }
    }
    ImGui::End();

    dockManager.RenderPanels();

    // Show intial popup
    static bool showNewProjectPopup = false;
    static bool showLoadProjectPopup = false;
    static bool isConfiguringProject = false;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0f);
    if (!ProjectConfig::getInstance().isConfigInitialized() && !isConfiguringProject)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::SetNextWindowPos(ImVec2((viewport->Size.x - 400) * 0.5f, (viewport->Size.y - 400) * 0.5f));
        if (ImGui::BeginPopupModal("Project setup", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
        {
            // TODO: Add image when kengine is in the 1.0
            ImGuiEngined::ImageWithoutBorder((void*)(intptr_t) splash_texture, ImVec2(400, 200));
            ImGui::NewLine();

            ImGui::Columns(2, NULL, false);

            // --- First column ---
            const char* projectActionsText = "Project actions";
            float textWidth = ImGui::CalcTextSize(projectActionsText).x;
            
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            ImGui::SetCursorPosX((ImGui::GetColumnWidth() - textWidth) * 0.5f);
            ImGui::Text(projectActionsText);
            ImGui::NewLine();

            ImGui::PopStyleColor();

            float columnWidth = ImGui::GetContentRegionAvail().x;
            float buttonWidth = 120.0f;
            float offsetX = (columnWidth - buttonWidth) * 0.5f;

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
            if (ImGui::Button((std::string(ICON_FA_FOLDER_PLUS) + " New Project").c_str(), ImVec2(buttonWidth, 0))) {
                ImGui::CloseCurrentPopup();
                showNewProjectPopup = true;
                isConfiguringProject = true;
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
            if (ImGui::Button((std::string(ICON_FA_FOLDER) + " Load Project").c_str(), ImVec2(buttonWidth, 0))) {
                ImGui::CloseCurrentPopup();
                showLoadProjectPopup = true;
                isConfiguringProject = true;
            }

            ImGui::NextColumn();

            // --- Second column ---
            const char* dummyActionsText = "Recent projects";
            textWidth = ImGui::CalcTextSize(dummyActionsText).x;

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8 + ((ImGui::GetColumnWidth() - textWidth) * 0.5)); // 8 is the padding
            ImGui::Text(dummyActionsText);
            ImGui::PopStyleColor();
            ImGui::NewLine();

            columnWidth = ImGui::GetContentRegionAvail().x;
            offsetX = (columnWidth - buttonWidth) * 0.5f;

            std::vector<ProjectConfig> recentProjects = ProjectConfig::getInstance().getRecentProjects(FileUtils::GetProjectsBaseFolder());

            // One Button for each recent project
            for (const ProjectConfig& project : recentProjects) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
                if (ImGui::Button(project.projectName.c_str(), ImVec2(buttonWidth, 0))) {
                    ProjectConfig& configInstance = ProjectConfig::getInstance();
                    configInstance.projectName = project.projectName;
                    configInstance.projectDirectory = project.projectDirectory;
                    configInstance.isInitialized = true;
                    dockManager.getWorkSceneController()->getSceneManager()->LoadScene();
                }
            }

            ImGui::Columns(1);
            ImGui::PopStyleColor();

            if (recentProjects.size() < 3)
                ImGui::NewLine();
            ImGui::NewLine();

            // Center the button
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth) * 0.5f);
            if (ImGui::Button((std::string(ICON_FA_HEART) + " Support us").c_str(), ImVec2(120, 0))) {
                OpenURL("https://github.com/sponsors/lapamanstudio");
            }
            ImGui::EndPopup();
        }

        ImGui::OpenPopup("Project setup");
    }

    if (showNewProjectPopup) {
        ImGui::SetNextWindowSize(ImVec2(400, 180));
        ImGui::OpenPopup("New Project");
        showNewProjectPopup = false;
    }

    if (showLoadProjectPopup) {
        ImGui::SetNextWindowSize(ImVec2(400, 180));
        ImGui::OpenPopup("Load Project");
        showLoadProjectPopup = false;
    }

    if (isConfiguringProject) 
        ImGui::SetNextWindowPos(ImVec2((viewport->Size.x - 400) * 0.5f, (viewport->Size.y - 180) * 0.5f));

    if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
    {
        static char projectName[128] = "New Project";
        static char projectDirectory[256] = "";
        static bool directoryModifiedByUser = false;
        static bool showError = false;
        static std::string errorMessage = "";

        auto generateUniqueProjectName = [](const std::string& baseName) -> std::string {
            int count = 0;
            std::string uniqueName = baseName;
            std::string basePath = FileUtils::GetProjectsBaseFolder();

            while (FileUtils::DirectoryExists(basePath + "/" + uniqueName)) {
                count++;
                uniqueName = baseName + " (" + std::to_string(count) + ")";
            }

            return uniqueName;
        };

        auto isValidProjectName = [](const std::string& name) -> bool {
            return !name.empty() && name.find_first_of("\\/:*?\"<>|") == std::string::npos;
        };

        if (ImGui::IsWindowAppearing()) {
            std::string uniqueProjectName = generateUniqueProjectName("New Project");
            strncpy(projectName, uniqueProjectName.c_str(), sizeof(projectName));
            projectName[sizeof(projectName) - 1] = '\0';

            std::string defaultDirectory = FileUtils::GetProjectsBaseFolder() + FileUtils::GetPathSeparator() + projectName;
            strncpy(projectDirectory, defaultDirectory.c_str(), sizeof(projectDirectory));
            projectDirectory[sizeof(projectDirectory) - 1] = '\0';

            directoryModifiedByUser = false;
            showError = false;
            errorMessage = "";
        }

        ImGui::Text("Project Name:");
        ImGui::SetNextItemWidth(350);
        if (ImGui::InputText("##projectName", projectName, IM_ARRAYSIZE(projectName)))
        {
            if (!directoryModifiedByUser) {
                std::string newProjectDirectory = FileUtils::GetProjectsBaseFolder() + FileUtils::GetPathSeparator() + std::string(projectName);
                strncpy(projectDirectory, newProjectDirectory.c_str(), sizeof(projectDirectory));
                projectDirectory[sizeof(projectDirectory) - 1] = '\0';
            }
            showError = !isValidProjectName(projectName);
            if (showError) {
                errorMessage = "Invalid project name! Avoid special characters.";
            }
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Avoid using special characters like \\ / : * ? \" < > |");

        ImGui::Text("Project Directory:");
        ImGui::SetNextItemWidth(350);
        if (ImGui::InputText("##projectDirectory", projectDirectory, IM_ARRAYSIZE(projectDirectory)))
        {
            directoryModifiedByUser = true;
        }

        if (showError) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text("%s", errorMessage.c_str());
            ImGui::PopStyleColor();
        }

        ImGui::NewLine();
        if (!showError) ImGui::NewLine();

        bool canCreateProject = !showError && isValidProjectName(projectName);
        if (!canCreateProject) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 0.3f));
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.4f, 0.0f, 1.0));
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.0f, 1.0));
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            if (FileUtils::DirectoryExists(projectDirectory)) {
                showError = true;
                errorMessage = "Directory already exists!";
            } else {
                if (!std::filesystem::exists(projectDirectory)) {
                    std::filesystem::create_directories(projectDirectory);
                    std::filesystem::create_directories(std::string(projectDirectory) + FileUtils::GetPathSeparator() + "assets");
                    std::filesystem::create_directories(std::string(projectDirectory) + FileUtils::GetPathSeparator() + "config");
                    std::filesystem::create_directories(std::string(projectDirectory) + FileUtils::GetPathSeparator() + "build");
                    std::filesystem::create_directories(std::string(projectDirectory) + FileUtils::GetPathSeparator() + "logs");
                }

                isConfiguringProject = false;
                ProjectConfig::getInstance().projectName = projectName;
                ProjectConfig::getInstance().projectDirectory = projectDirectory;
                ProjectConfig::getInstance().isInitialized = true;
                ProjectConfig::getInstance().saveToFile(std::string(projectDirectory) + FileUtils::GetPathSeparator() + "project.json");
                ProjectConfig::getInstance().addRecentProject(FileUtils::GetProjectsBaseFolder());
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::PopStyleColor(2);
        if (!canCreateProject) {
            ImGui::PopItemFlag();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            isConfiguringProject = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleVar(); // window rounding

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cleanup_window() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
