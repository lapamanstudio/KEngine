#include "window/gui/panels/ConsolePanel.h"

ConsolePanel::ConsolePanel(DockManager* dockManager) : dockManager(dockManager) {}

void ConsolePanel::render(int posX, int posY, int width, int height) {
    char buffer[256];
    strncpy(buffer, "Hello, world!", sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
    ImGui::SetKeyboardFocusHere();
    ImGui::InputText("##consoleInput", buffer, sizeof(buffer));
    ImGui::PopStyleColor();
}

ConsolePanel::~ConsolePanel() {}
