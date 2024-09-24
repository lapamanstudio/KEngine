#include "core/utils/StringUtils.h"
#include <cctype>

std::string StringUtils::TruncateTextToWidth(const std::string& text, float maxWidth) {
    if (text.empty()) return "";

    std::string displayText = text;
    float textWidth = ImGui::CalcTextSize(displayText.c_str()).x;

    if (textWidth <= maxWidth) {
        return displayText;
    }

    std::string ellipsis = "...";
    float ellipsisWidth = ImGui::CalcTextSize(ellipsis.c_str()).x;

    if (maxWidth <= ellipsisWidth) {
        return ellipsis;
    }

    int end = static_cast<int>(displayText.length()) - 1;
    int start = 0;

    while (start <= end) {
        int mid = (start + end) / 2;
        displayText = text.substr(0, mid) + ellipsis;
        textWidth = ImGui::CalcTextSize(displayText.c_str()).x;

        if (textWidth > maxWidth) {
            end = mid - 1;
        } else {
            start = mid + 1;
        }
    }

    displayText = text.substr(0, end) + ellipsis;
    return displayText;
}

int StringUtils::FilenameFilter(ImGuiInputTextCallbackData* data) {
    if (data->EventChar < 256) {
        const char* invalidChars = "\\/:*?\"<>|";
        if (strchr(invalidChars, (char)data->EventChar) != nullptr) {
            return true;
        }
    }
    return false;
}

