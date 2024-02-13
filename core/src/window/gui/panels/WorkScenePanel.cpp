#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "window/gui/panels/WorkScenePanel.h"
#include "window/gui/renderer/WorkSceneRenderer.h"

#define PADDING_LEFT_RIGHT 8
#define PADDING_TOP_BOTTOM 8

WorkSceneRenderer* workSceneRenderer = nullptr;
ImVec2 padding = ImVec2(PADDING_LEFT_RIGHT, PADDING_TOP_BOTTOM);

void ImageWithoutBorder(ImTextureID user_texture_id, const ImVec2& image_size, const ImVec2& padding_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0))
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + image_size.x, window->DC.CursorPos.y + image_size.y));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
        return;

    window->DrawList->AddImage(user_texture_id, ImVec2(bb.Min.x - padding_size.x, bb.Min.y - padding_size.y), ImVec2(bb.Max.x, bb.Max.y + padding_size.y + 1), uv0, uv1, ImGui::GetColorU32(tint_col));
}

void WorkScenePanel::Render(int posX, int posY, int width, int height) {
    if (width - PADDING_LEFT_RIGHT <= 0 || height - PADDING_TOP_BOTTOM <= 0) return;

    ImGuiDockNode* node = ImGui::GetCurrentWindow()->DockNode;

    if (!workSceneRenderer) {
        workSceneRenderer = new WorkSceneRenderer(0, 0, 800, 600);
    }

    // Update size to be rendered
    workSceneRenderer->updateSize(PADDING_LEFT_RIGHT / 2, 0, width - 12, height - (node->IsHiddenTabBar() ? 16 : 35));

    // Render the scene
    workSceneRenderer->render();

    GLuint texID = workSceneRenderer->getTexture();
    ImVec2 imageSize(width - 12, height - (node->IsHiddenTabBar() ? 16 : 35)); // 35 = padding size + header size
    ImageWithoutBorder((void*)(intptr_t)texID, imageSize, padding);
}
