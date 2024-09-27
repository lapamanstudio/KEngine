#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <imgui.h>
#include <imgui_internal.h>

#include <string>
#include <vector>

class StringUtils {
public:
    static std::string TruncateTextToWidth(const std::string& text, float maxWidth);
    static int FilenameFilter(ImGuiInputTextCallbackData* data);
};

#endif  // STRING_UTILS_H
