#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class WindowManager {
public:
    static WindowManager& getInstance() {
        static WindowManager instance;
        return instance;
    }

    GLFWwindow* getWindow() const {
        return window;
    }

    void setWindow(GLFWwindow* newWindow) {
        window = newWindow;
    }

private:
    WindowManager() : window(nullptr) {}
    ~WindowManager() = default;

    GLFWwindow* window;
};

#endif // WINDOW_MANAGER_H
