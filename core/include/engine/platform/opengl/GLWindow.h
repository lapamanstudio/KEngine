#ifndef ENGINE_GLWINDOW_H
#define ENGINE_GLWINDOW_H

#include "engine/graphics/Window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

class GLWindow : public Window {
public:
    GLWindow();
    ~GLWindow();

    void Init(int width, int height, const char* title) override;
    void PollEvents() override;
    void Clear() override;
    void SwapBuffers() override;
    bool ShouldClose() override;
    float GetTime() override;

private:
    GLFWwindow* window;
};

#endif //ENGINE_GLWINDOW_H
