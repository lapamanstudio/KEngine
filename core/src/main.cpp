// main.c
#include <GL/glew.h>
#include <GLFW/glfw3.h> // Assuming GLFW for window creation
#include "window/window.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(void) {
    // Initialize the library
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(640, 480, "KEngine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwMaximizeWindow(window);

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
