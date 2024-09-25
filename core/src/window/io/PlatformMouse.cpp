#include "window/io/PlatformMouse.h"

#include "window/WindowManager.h"
#include <imgui.h>
#include <imgui_internal.h>

void WrapMousePos() {
    ImGuiContext* g = ImGui::GetCurrentContext();
    ImGuiIO& io = g->IO;
    ImVec2& mousePos = io.MousePos;

    GLFWwindow* window = WindowManager::getInstance().getWindow();
    int windowPosX, windowPosY;
    glfwGetWindowPos(window, &windowPosX, &windowPosY);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // io.MousePos is relative from window, so (0, 0)
    ImVec2 windowMin = ImVec2(0, 0);
    ImVec2 windowMax = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    bool wrapped = false;
    const float offset = 1.0f;

    // Horizontal wrapping
    if (io.MouseDelta.x < 0 && mousePos.x <= windowMin.x) {
        glfwSetCursorPos(window, windowPosX + windowMax.x - offset, windowPosY + mousePos.y);
        mousePos.x = windowMax.x - offset;
        wrapped = true;
    } else if (io.MouseDelta.x > 0 && mousePos.x >= windowMax.x - offset) {
        glfwSetCursorPos(window, windowPosX + windowMin.x + offset, windowPosY + mousePos.y);
        mousePos.x = windowMin.x + offset;
        wrapped = true;
    }

    // Vertical wrapping
    if (io.MouseDelta.y < 0 && mousePos.y <= windowMin.y) {
        glfwSetCursorPos(window, windowPosX + mousePos.x, windowPosY + windowMax.y - offset);
        mousePos.y = windowMax.y - offset;
        wrapped = true;
    } else if (io.MouseDelta.y > 0 && mousePos.y >= windowMax.y - offset) {
        glfwSetCursorPos(window, windowPosX + mousePos.x, windowPosY + windowMin.y + offset);
        mousePos.y = windowMin.y + offset;
        wrapped = true;
    }

    // Update IO if wrapping occurred
    if (wrapped) {
        io.MousePosPrev = mousePos;
        io.MouseDelta = ImVec2(0, 0);
        io.WantSetMousePos = true;
    }
}