#include "window/gui/panels/controller/WorkSceneController.h"
#include "graphics/scene/objects/Camera.h"

WorkSceneController::WorkSceneController(int x, int y, int w, int h) 
    : workSceneRenderer(std::make_shared<WorkSceneRenderer>(&isDebugging, x, y, w, h)),
      camera(std::make_shared<SceneCamera>()),
      sceneManager(std::make_shared<SceneManager>()),
      isDebugging(false),
      isMouseDragging(false),
      isMouseSelecting(false),
      lastMouseX(0.0),
      lastMouseY(0.0),
      posX(x),
      posY(y),
      width(w),
      height(h),
      isF3Pressed(false) {
    sceneManager->AddObject(std::make_shared<Camera>(0, 0, 800, 600));
    camera->Move(glm::vec2(-10, -10));

    workSceneRenderer->updateSize(camera.get(), 4, 0, w, h); // PADDING_LEFT_RIGHT / 2, 0

    crossCursor = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
}
WorkSceneController::~WorkSceneController() {
    glfwDestroyCursor(crossCursor);
}

void WorkSceneController::update(GLFWwindow* window, float mouseWheel, float deltaTime) {
    processKeyboardInput(window, deltaTime);
    processMouseInput(window, mouseWheel, deltaTime);
}

void WorkSceneController::render(int x, int y, int w, int h) {
    workSceneRenderer->updateSize(camera.get(), 4, 0, w, h); // PADDING_LEFT_RIGHT / 2, 0
    posX = x;
    posY = y;
    width = w;
    height = h;
    workSceneRenderer->render(camera.get(), sceneManager.get());
}

void WorkSceneController::processKeyboardInput(GLFWwindow* window, float deltaTime) {
    // Keyboard movement
    float moveSpeed = 200.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        this->MoveCamera(moveSpeed * (2.3f - camera->GetZoom()), 0);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        this->MoveCamera(0, moveSpeed * (2.3f - camera->GetZoom()));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        this->MoveCamera(-moveSpeed * (2.3f - camera->GetZoom()), 0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        this->MoveCamera(0, -moveSpeed * (2.3f - camera->GetZoom()));
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

void WorkSceneController::processMouseInput(GLFWwindow* window, float mouseWheel, float deltaTime) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glm::vec2 mousePos(mouseX - posX, mouseY - posY);
    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        if (!isMouseDragging) {
            isMouseDragging = true;
            lastMouseX = mousePos.x; 
            lastMouseY = mousePos.y;
        } else {
            glfwSetCursor(window, crossCursor);

            // Calculate the mouse movement
            double deltaX = mousePos.x - lastMouseX;
            double deltaY = mousePos.y - lastMouseY;

            // Adjust the camera based on mouse movement
            this->MoveCamera(static_cast<float>(-deltaX / camera->GetZoom()), static_cast<float>(deltaY / camera->GetZoom()));

            lastMouseX = mousePos.x;
            lastMouseY = mousePos.y;
            return;
        }
    } else {
        // Release
        if (isMouseDragging) {
            isMouseDragging = false;
            glfwSetCursor(window, nullptr);
        }
    }

    // Reverse the Y axis
    mousePos.y = height - mousePos.y + 32;

    // Select objects logic
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isMouseDragging) {
        // Check if mouse in work scene
        if (mousePos.x < 0 || mousePos.x > width || mousePos.y < 0 || mousePos.y > height) {
            isMouseSelectBlocked = true;
            return;
        }

        if (isMouseSelectBlocked)
            return;

        if (!isMouseSelecting) {
            isMouseSelecting = true;

            lastMouseX = camera->screenToWorld(mousePos, glm::vec2(width, height)).x;
            lastMouseY = camera->screenToWorld(mousePos, glm::vec2(width, height)).y;
        } else {
            // Get mouse coordinates in the camera
            double finalMouseX = camera->screenToWorld(mousePos, glm::vec2(width, height)).x;
            double finalMouseY = camera->screenToWorld(mousePos, glm::vec2(width, height)).y;

            // Draw selection rectangle
            workSceneRenderer->setSelectionBox(glm::vec4(lastMouseX, lastMouseY, finalMouseX, finalMouseY));
        }
    } else {
        isMouseSelectBlocked = false;

        if (isMouseSelecting) {
            isMouseSelecting = false;

            // Get mouse coordinates in the camera
            double finalMouseX = camera->screenToWorld(mousePos, glm::vec2(width, height)).x;
            double finalMouseY = camera->screenToWorld(mousePos, glm::vec2(width, height)).y;
            
            // Select object
            std::unique_ptr<std::vector<std::shared_ptr<GameObject>>> filteredObjects;
            if (((int) finalMouseX == (int)lastMouseX) && ((int) finalMouseY == (int)lastMouseY)) {
                filteredObjects = sceneManager->GetObjectsInCoords(glm::vec2(finalMouseX, finalMouseY));
            } else {
                filteredObjects = sceneManager->GetObjectsInCoords(glm::vec4(lastMouseX, lastMouseY, finalMouseX, finalMouseY));
            }

            if (filteredObjects->size() == 0)
                sceneManager->SetActiveObject(nullptr);
            else if (filteredObjects->size() == 1)
                sceneManager->SetActiveObject(filteredObjects->at(0));
            else
                printf("Multiple objects selected\n");

            workSceneRenderer->clearSelectionBox();
        }
    }

    // Zoom In/Out
    if (mouseWheel != 0) {
        float zoomFactor = mouseWheel * 0.1f;
        float currentZoom = camera->GetZoom();
        float newZoom = currentZoom + zoomFactor;

        // Constrain zoom level
        if (newZoom > 0.1f && newZoom < 2.0f) {
            // Normalize mouse position between -1 and 1
            glm::vec2 mousePosNormalize = glm::vec2(
                (mousePos.x - width / 2.0f) / (width / 2.0f),
                (mousePos.y - height / 2.0f) / (height / 2.0f)
            );

            glm::vec2 cameraPos = camera->GetPosition();

            // Calculate the world coordinates of the mouse position
            glm::vec2 worldMousePos = cameraPos + mousePosNormalize * (width / (2.0f * currentZoom));

            // Calculate the new camera position to maintain the zoom focal point
            glm::vec2 newCameraPos = worldMousePos - mousePosNormalize * (width / (2.0f * newZoom));

            // Smooth interpolation of the camera position
            cameraPos = glm::mix(cameraPos, newCameraPos, 0.1f);

            // Set the new camera position and zoom
            this->camera->SetPosition(cameraPos);
            this->camera->SetZoom(newZoom);
        }
    }
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

std::shared_ptr<SceneCamera> WorkSceneController::getCamera() {
    return camera;
}
