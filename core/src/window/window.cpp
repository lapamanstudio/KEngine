#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window/Window.h"
#include "window/gui/DockManager.h"

DockManager dockManager;

void initialize_window(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Enable docking
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.IniFilename = NULL; // Disable INI file saving

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);  // Dark background
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // Dark title bar background
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Slightly brighter title bar when active
    style.Colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Dark separator color

    style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f); // Dark header background
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Hovered header background
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Active header background

    style.Colors[ImGuiCol_Separator] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f); // Slightly brighter separator color
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Hovered separator color
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Active separator color

    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.8f, 0.8f, 0.8f, 0.10f); // Grip color
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.60f); // Hovered grip color

    style.Colors[ImGuiCol_Tab] = ImLerp(style.Colors[ImGuiCol_Header], style.Colors[ImGuiCol_TitleBgActive], 0.80f); // Dark tab color
    style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_HeaderHovered]; // Hovered tab color
    style.Colors[ImGuiCol_TabActive] = ImLerp(style.Colors[ImGuiCol_HeaderActive], style.Colors[ImGuiCol_TitleBgActive], 0.60f); // Active tab color
    style.Colors[ImGuiCol_TabUnfocused] = ImLerp(style.Colors[ImGuiCol_Tab], style.Colors[ImGuiCol_TitleBg], 0.80f); // Unfocused tab color
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImLerp(style.Colors[ImGuiCol_TabActive], style.Colors[ImGuiCol_TitleBg], 0.40f); // Unfocused active tab color

    style.Colors[ImGuiCol_DockingPreview] = style.Colors[ImGuiCol_Header]; // Docking preview color
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
    ImGui::DockBuilderDockWindow(P_PROJECT_FILES, dock_id_left);
    ImGui::DockBuilderDockWindow(P_CONSOLE, dock_id_down);
    ImGui::DockBuilderDockWindow(P_OBJECT_INSPECTOR, dock_id_right);
    ImGui::DockBuilderDockWindow(P_WORLD_PROPERTIES, dock_id_right);

    // The central area is automatically determined by the docking system
    ImGui::DockBuilderDockWindow(P_WORK_SCENE, dockspace_id);

    ImGui::DockBuilderFinish(dockspace_id);
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
    const ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

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
