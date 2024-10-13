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

    this->width = width;
    this->height = height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        GLWindow* glWin = static_cast<GLWindow*>(glfwGetWindowUserPointer(win));

        glWin->width = w;
        glWin->height = h;
        glViewport(0, 0, w, h);

        if (glWin->resizeCallback) {
            glWin->resizeCallback(w, h);
        }
    });

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

int GLWindow::GetWidth() const {
    return width;
}

int GLWindow::GetHeight() const {
    return height;
}

void GLWindow::SetResizeCallback(WindowResizeCallback callback) {
    resizeCallback = callback;
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        GLWindow* glWin = static_cast<GLWindow*>(glfwGetWindowUserPointer(win));
        glWin->width = w;
        glWin->height = h;
        glViewport(0, 0, w, h);

        if (glWin->resizeCallback) {
            glWin->resizeCallback(w, h);
        }
    });
}
