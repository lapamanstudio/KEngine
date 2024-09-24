#ifndef IMGUIUTILS_H
#define IMGUIUTILS_H

#include <imgui.h>
#include <imgui_internal.h>

#include <string>

class ImGuiEngined {
public:
    static void ImageWithoutBorder(ImTextureID user_texture_id, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0)); 
    static bool ButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags, ImVec4 text_hovered_color);
    static bool InputTextStdString(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
};


#endif // IMGUIUTILS_H