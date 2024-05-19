#include "window/gui/panels/controller/WorkSceneController.h"
#include "graphics/scene/objects/Camera.h"

WorkSceneController::WorkSceneController(int x, int y, int w, int h) {
    workSceneRenderer = std::make_shared<WorkSceneRenderer>(0, 0, w, h);
    camera = std::make_shared<SceneCamera>();
    sceneManager = std::make_shared<SceneManager>();

    sceneManager->AddObject(std::make_shared<Camera>(0, 0, 200, 100));
}

WorkSceneController::~WorkSceneController() {}

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
    workSceneRenderer->render(camera.get(), sceneManager.get());
}

void WorkSceneController::MoveCamera(float x, float y) {
    camera->Move(glm::vec2(x, y));
}

GLuint WorkSceneController::getTexture() {
    return workSceneRenderer->getTexture();
}