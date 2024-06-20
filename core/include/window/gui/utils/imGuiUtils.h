#ifndef IMGUIUTILS_H
#define IMGUIUTILS_H

#include <imgui.h>
#include <imgui_internal.h>

void ImageWithoutBorder(ImTextureID user_texture_id, const ImVec2& image_size, const ImVec2& padding_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
void MakeTabVisible(const char* window_name);

#endif // IMGUIUTILS_H