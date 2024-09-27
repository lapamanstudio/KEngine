#include "editor/window/gui/utils/imGuiUtils.h"

// ImGui don't contains a method to place an image without borders, then this is the ImGui::Image version without borders
// You must add as parameter the padding from the sides to remove it when adding the image to be drawn
void ImGuiEngined::ImageWithoutBorder(ImTextureID user_texture_id, const ImVec2& image_size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImVec2 padding_size = ImVec2(8, 8);
    const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + image_size.x, window->DC.CursorPos.y + image_size.y));
    
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
        return;

    // Draw the image without borders, adjusted for padding automatically
    window->DrawList->AddImage(user_texture_id,
        ImVec2(bb.Min.x - padding_size.x, bb.Min.y - padding_size.y),
        ImVec2(bb.Max.x, bb.Max.y + padding_size.y),
        uv0, uv1, ImGui::GetColorU32(tint_col));
}

// ImGui don't contains a method to put buttons with custom text color when hovered
bool ImGuiEngined::ButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags, ImVec4 text_hovered_color)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    ImGui::RenderNavHighlight(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

    if (g.LogEnabled)
        ImGui::LogSetNextTextDecoration("[", "]");
    if (hovered) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(text_hovered_color));
    ImGui::RenderTextClipped(ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y), ImVec2(bb.Max.x - style.FramePadding.x, bb.Max.y - style.FramePadding.y), label, NULL, &label_size, style.ButtonTextAlign, &bb);
    if (hovered) ImGui::PopStyleColor();
    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImGuiEngined::InputTextStdString(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {
    char buf[256];
    strncpy(buf, str->c_str(), sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';

    bool changed = ImGui::InputText(label, buf, sizeof(buf), flags, callback, user_data);

    if (changed) {
        *str = buf;
    }
    return changed;
}
