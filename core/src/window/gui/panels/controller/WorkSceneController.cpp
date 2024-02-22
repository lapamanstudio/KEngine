#include "window/gui/panels/controller/WorkSceneController.h"

WorkSceneController::WorkSceneController(int x, int y, int w, int h) {
    workSceneRenderer = new WorkSceneRenderer(0, 0, w, h);
    camera = new Camera();
}

WorkSceneController::~WorkSceneController() {
    delete workSceneRenderer;
    delete camera;
}

void WorkSceneController::update(GLFWwindow* window) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        if (!isMouseDragging) {
            isMouseDragging = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        } else {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // Calculate the mouse movement
            double deltaX = mouseX - lastMouseX;
            double deltaY = mouseY - lastMouseY;

            // Adjust the camera based on mouse movement
            this->MoveCamera(static_cast<float>(deltaX), static_cast<float>(deltaY));

            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }
    } else {
        // Release
        isMouseDragging = false;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        this->MoveCamera(-1, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        this->MoveCamera(0, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        this->MoveCamera(1, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        this->MoveCamera(0, -1);
    }
}

void WorkSceneController::render(int x, int y, int w, int h) {
    workSceneRenderer->updateSize(x, y, w, h);
    workSceneRenderer->render(camera);
}

void WorkSceneController::MoveCamera(float x, float y) {
    camera->Move(glm::vec2(x, y));
}

GLuint WorkSceneController::getTexture() {
    return workSceneRenderer->getTexture();
}