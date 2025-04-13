#include "engine/core/input/Input.h"

#include <GLFW/glfw3.h>

void GLKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const char* name = glfwGetKeyName(key, scancode);
    std::string keyStr;

    if (name) {
        keyStr = std::string(name); // In lowercase
    } else {
        switch (key) {
            case GLFW_KEY_SPACE: keyStr = "Space"; break;
            case GLFW_KEY_ENTER: keyStr = "Enter"; break;
            case GLFW_KEY_BACKSPACE: keyStr = "Backspace"; break;
            case GLFW_KEY_TAB: keyStr = "Tab"; break;
            case GLFW_KEY_ESCAPE: keyStr = "Escape"; break;
            case GLFW_KEY_LEFT: keyStr = "Left"; break;
            case GLFW_KEY_RIGHT: keyStr = "Right"; break;
            case GLFW_KEY_UP: keyStr = "Up"; break;
            case GLFW_KEY_DOWN: keyStr = "Down"; break;
            case GLFW_KEY_LEFT_CONTROL: keyStr = "LeftCtrl"; break;
            case GLFW_KEY_RIGHT_CONTROL: keyStr = "RightCtrl"; break;
            case GLFW_KEY_LEFT_SHIFT: keyStr = "LeftShift"; break;
            case GLFW_KEY_RIGHT_SHIFT: keyStr = "RightShift"; break;
            case GLFW_KEY_LEFT_ALT: keyStr = "LeftAlt"; break;
            case GLFW_KEY_RIGHT_ALT: keyStr = "RightAlt"; break;
            case GLFW_KEY_DELETE: keyStr = "Delete"; break;
            case GLFW_KEY_F1: keyStr = "F1"; break;
            case GLFW_KEY_F2: keyStr = "F2"; break;
            case GLFW_KEY_F3: keyStr = "F3"; break;
            case GLFW_KEY_F4: keyStr = "F4"; break;
            case GLFW_KEY_F5: keyStr = "F5"; break;
            case GLFW_KEY_F6: keyStr = "F6"; break;
            case GLFW_KEY_F7: keyStr = "F7"; break;
            case GLFW_KEY_F8: keyStr = "F8"; break;
            case GLFW_KEY_F9: keyStr = "F9"; break;
            case GLFW_KEY_F10: keyStr = "F10"; break;
            case GLFW_KEY_F11: keyStr = "F11"; break;
            case GLFW_KEY_F12: keyStr = "F12"; break;
            default: keyStr = "Unknown_" + std::to_string(key); break;
        }
    }

    bool isPressed = action != GLFW_RELEASE;
    Input::SetKeyState(keyStr, isPressed);
}
