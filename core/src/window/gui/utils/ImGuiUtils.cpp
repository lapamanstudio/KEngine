#include "window/gui/utils/imGuiUtils.h"

// ImGui don't contains a method to place an image without borders, then this is the ImGui::Image version without borders
// You must add as parameter the padding from the sides to remove it when adding the image to be drawn
void ImageWithoutBorder(ImTextureID user_texture_id, const ImVec2& image_size, const ImVec2& padding_size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
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
