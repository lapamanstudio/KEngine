#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window/WindowManager.h"
#include "window/gui/utils/imGuiUtils.h"
#include "window/gui/panels/WorkScenePanel.h"
#include "window/gui/panels/workscene/renderer/WorkSceneRenderer.h"

#define PADDING_LEFT_RIGHT 8
#define PADDING_TOP_BOTTOM 8

ImVec2 padding = ImVec2(PADDING_LEFT_RIGHT, PADDING_TOP_BOTTOM);

static double lastFrameTime = 0.0;

WorkScenePanel::WorkScenePanel(DockManager* dockManager) : dockManager(dockManager) {}

void WorkScenePanel::render(int posX, int posY, int width, int height) {
    if (width - PADDING_LEFT_RIGHT <= 0 || height - PADDING_TOP_BOTTOM <= 0) return;
    ImGui::SetScrollX(0);

    if (!workSceneController) {
        workSceneController = new WorkSceneController(posX, posY, width, height);
        dockManager->setWorkSceneController(workSceneController);
        lastFrameTime = glfwGetTime();
    }

    int w = width - 12;
    int h = height - 38; // 38 = Adjust of header size

    ImGuiIO& io = ImGui::GetIO();
    
    // Get if the mouse is inside the panel
    bool mouseInPanel = io.MousePos.x >= posX - 20 && io.MousePos.x <= posX - 20 + width && io.MousePos.y >= posY && io.MousePos.y <= posY + height && ImGui::IsWindowFocused();

    // If not focus and click in the panel with click that is not left click, focus the panel
    if (ImGui::IsWindowHovered() && (io.MouseClicked[1] || io.MouseClicked[2]))
        ImGui::SetWindowFocus();

    // DeltaTime
    double currentFrameTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
    lastFrameTime = currentFrameTime;

    workSceneController->update(WindowManager::getInstance().getWindow(), mouseInPanel ? ImGui::GetIO().MouseWheel : 0, deltaTime, mouseInPanel);
    workSceneController->render(posX - 7, posY + 21, w + 10, h + 16); // Adjust to final canvas limits

    GLuint texID = workSceneController->getTexture();
    ImVec2 imageSize(w, h);
    ImGuiEngined::ImageWithoutBorder((void*)(intptr_t)texID, imageSize);
}

WorkScenePanel::~WorkScenePanel() {
    delete workSceneController;
}
