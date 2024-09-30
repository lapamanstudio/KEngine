#include "engine/platform/opengl/GLWindow.h"

GLWindow::GLWindow() : window(nullptr) {}

GLWindow::~GLWindow() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void GLWindow::Init(int width, int height, const char* title) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    glfwSwapInterval(1); // Enable vsync
}

void GLWindow::PollEvents() {
    glfwPollEvents();
}

void GLWindow::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLWindow::SwapBuffers() {
    glfwSwapBuffers(window);
}

bool GLWindow::ShouldClose() {
    return glfwWindowShouldClose(window);
}

float GLWindow::GetTime() {
    return static_cast<float>(glfwGetTime());
}
