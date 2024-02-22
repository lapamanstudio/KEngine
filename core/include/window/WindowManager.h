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