// main.c
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include "config.h"
#include "window/Window.h"
#include "window/WindowManager.h"

void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(void) {
    printf("Running KEngine %ls\n", GAME_ENGINE_VERSION);
    
    // Set the GLFW error callback before initialization
    glfwSetErrorCallback(error_callback);
    
    // Initialize the library
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(640, 480, "KEngine", NULL, NULL);
    WindowManager::getInstance().setWindow(window);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwMaximizeWindow(window);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLenum err = glewInit();
    if (err != GLEW_OK)
        return -1;

    // Initialize ImGui window
    initialize_window(window);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Render ImGui window
        render_window();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Cleanup
    cleanup_window();
    glfwTerminate();
    return 0;
}
