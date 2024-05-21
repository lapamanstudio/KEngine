#include "window/gui/panels/controller/WorkSceneController.h"
#include "graphics/scene/objects/Camera.h"

WorkSceneController::WorkSceneController(int x, int y, int w, int h) 
    : isDebugging(false), isF3Pressed(false), posX(x), posY(y), width(w), height(h) {
    workSceneRenderer = std::make_shared<WorkSceneRenderer>(&isDebugging, x, y, w, h);
    camera = std::make_shared<SceneCamera>();
    sceneManager = std::make_shared<SceneManager>();

    sceneManager->AddObject(std::make_shared<Camera>(0, 0, 800, 600));
}

WorkSceneController::~WorkSceneController() {}

void WorkSceneController::update(GLFWwindow* window, float mouseWheel) {
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
            this->MoveCamera(static_cast<float>(-deltaX), static_cast<float>(deltaY));

            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }
    } else {
        // Release
        isMouseDragging = false;
    }

    // Zoom In/Out
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glm::vec2 mousePos(mouseX - posX, mouseY - posY);
    
    if (mouseWheel != 0) {
        float zoomFactor = mouseWheel / 10.0f;
        float newZoom = camera->GetZoom() + zoomFactor;

        // Normalize between -1 and 1
        glm::vec2 mousePosNormalize = glm::vec2(
            (mousePos.x - width / 2) / (width / 2),
            (mousePos.y - height / 2) / (height / 2)
        );

        // Constrain zoom level
        if (newZoom >= 0.1f && newZoom <= 2.0f) {
            glm::vec2 cameraPos = camera->GetPosition();

            // Adjust the camera position to maintain the zoom focal point
            glm::vec2 newCameraPos = glm::vec2(
                cameraPos.x + (mousePosNormalize.x * zoomFactor * cameraPos.x),
                cameraPos.y + (mousePosNormalize.y * zoomFactor * cameraPos.y)
            );

            // Update camera zoom
            this->ModifyZoom(zoomFactor);

            // Set the new camera position
            this->camera->SetPosition(newCameraPos);
        }
    }

    // Keyboard movement
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

    // Debug key - F3
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
        if (!isF3Pressed) {
            this->isDebugging = !this->isDebugging;
            isF3Pressed = true;
        }
    } else {
        isF3Pressed = false;
    }
}

void WorkSceneController::render(int x, int y, int w, int h) {
    workSceneRenderer->updateSize(4, 0, w, h); // PADDING_LEFT_RIGHT / 2, 0
    posX = x;
    posY = y;
    width = w;
    height = h;
    workSceneRenderer->render(camera.get(), sceneManager.get());
}

void WorkSceneController::MoveCamera(float x, float y) {
    camera->Move(glm::vec2(x, y));
}

void WorkSceneController::ModifyZoom(float factor) {
    camera->Zoom(factor);
}

GLuint WorkSceneController::getTexture() {
    return workSceneRenderer->getTexture();
}

bool WorkSceneController::isDebug() const {
    return isDebugging;
}
