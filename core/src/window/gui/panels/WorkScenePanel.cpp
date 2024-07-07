#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window/WindowManager.h"
#include "window/gui/utils/imGuiUtils.h"
#include "window/gui/panels/WorkScenePanel.h"
#include "window/gui/renderer/WorkSceneRenderer.h"

#define PADDING_LEFT_RIGHT 8
#define PADDING_TOP_BOTTOM 8

ImVec2 padding = ImVec2(PADDING_LEFT_RIGHT, PADDING_TOP_BOTTOM);

static double lastFrameTime = 0.0;

WorkScenePanel::WorkScenePanel(DockManager* dockManager) : dockManager(dockManager) {}

void WorkScenePanel::render(int posX, int posY, int width, int height) {
    if (width - PADDING_LEFT_RIGHT <= 0 || height - PADDING_TOP_BOTTOM <= 0) return;
    ImGui::SetScrollX(0);

    ImGuiDockNode* node = ImGui::GetCurrentWindow()->DockNode;

    if (!workSceneController) {
        workSceneController = new WorkSceneController(posX, posY, width, height);
        dockManager->setWorkSceneController(workSceneController);
        lastFrameTime = glfwGetTime();
    }

    int w = width - 12;
    int h = height - (node && node->IsHiddenTabBar() ? 16 : 35) - 4; // 35 = padding size + header size

    ImGuiIO& io = ImGui::GetIO();
    
    // Get if the mouse is inside the panel
    bool mouseInPanel = io.MousePos.x >= posX - 20 && io.MousePos.x <= posX - 20 + width && io.MousePos.y >= posY && io.MousePos.y <= posY + height;

    // DeltaTime
    double currentFrameTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
    lastFrameTime = currentFrameTime;

    workSceneController->update(WindowManager::getInstance().getWindow(), mouseInPanel ? ImGui::GetIO().MouseWheel : 0, deltaTime);
    workSceneController->render(posX, posY, w, h);

    GLuint texID = workSceneController->getTexture();
    ImVec2 imageSize(w, h);
    ImageWithoutBorder((void*)(intptr_t)texID, imageSize, padding);
}
