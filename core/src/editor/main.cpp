#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <atomic>
#include <cstdio>
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>

#include "editor/config.h"
#include "editor/core/ProjectConfig.h"
#include "editor/window/Window.h"
#include "editor/window/WindowManager.h"

std::atomic<bool> needs_render{true};

void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void file_drop_callback(GLFWwindow* window, int count, const char** paths);

int main(void) {
    printf("Running KEngine %ls\n", GAME_ENGINE_VERSION);
    
    // Set the GLFW error callback before initialization
    glfwSetErrorCallback(error_callback);
    
    // Initialize the library
    if (!glfwInit()) {
        printf("Failed to initialize GLFW. Error E001\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(640, 480, "KEngine", NULL, NULL);
    WindowManager::getInstance().setWindow(window);
    if (!window) {
        glfwTerminate();
        printf("Failed to create window. Error E002\n");
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
    if (err != GLEW_OK) {
        printf("Failed to initialize GLEW. Error E003: %s\n", glewGetErrorString(err));
        return -1;
    }

    // Initialize ImGui window
    initialize_window(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetDropCallback(window, file_drop_callback);

    const double frame_time_limit = 1.0 / 144; // 60 FPS
    auto last_time = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        bool render_this_frame = false;
        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed_time = std::chrono::duration<double>(current_time - last_time).count();

        int is_iconified = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
        int is_visible = glfwGetWindowAttrib(window, GLFW_VISIBLE);

        if (is_iconified || !is_visible) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            glfwPollEvents();
            continue;
        }

        if (needs_render.load() || elapsed_time >= frame_time_limit) {
            render_this_frame = true;
            last_time = current_time;
        }

        if (render_this_frame) {
            // Render ImGui window
            render_window();

            // Swap front and back buffers
            glfwSwapBuffers(window);
            
            needs_render.store(false);
        }

        if (render_this_frame) {
            glfwPollEvents();
        } else {
            glfwWaitEventsTimeout(frame_time_limit);
        }
    }

    // Cleanup
    cleanup_window();
    glfwTerminate();
    return 0;
}

// Copy files to project directory
void file_drop_callback(GLFWwindow* window, int count, const char** paths) {
    if (ProjectConfig::getInstance().isInitialized) {
        for (int i = 0; i < count; ++i) {
            fs::path sourcePath = fs::path(paths[i]);
            fs::path destinationPath = ProjectConfig::getInstance().getFullCurrentDirectory() / sourcePath.filename();
            
            try {
                if (fs::exists(destinationPath)) {
                    fs::remove(destinationPath);
                }

                fs::copy_file(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error copying file: " << e.what() << " [" << sourcePath << "] to [" << destinationPath << "]" << std::endl;
            }
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    needs_render.store(true);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    needs_render.store(true);
}
