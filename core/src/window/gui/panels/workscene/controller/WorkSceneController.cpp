#include "window/gui/panels/workscene/controller/WorkSceneController.h"
#include "window/gui/panels/workscene/gui/WorkSceneUI.h"
#include "graphics/drivers/ShaderHelper.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/TextureManager.h"
#include "graphics/scene/objects/Camera.h"
#include "graphics/icons/translation.h"
#include "graphics/icons/free_camera.h"

WorkSceneController::WorkSceneController(int x, int y, int w, int h) 
    : shader(std::make_shared<GLHelper>(vertexShaderSource, fragmentShaderSource, textShaderSource, textFragmentShader)),
      workSceneRenderer(std::make_shared<WorkSceneRenderer>(shader, x, y, w, h)),
      sceneManager(std::make_shared<SceneManager>()),
      workSceneUI(std::make_shared<WorkSceneUI>()),
      isMouseDragging(false),
      isMouseSelecting(false),
      lastMouseX(0.0),
      lastMouseY(0.0),
      posX(x),
      posY(y),
      width(w),
      height(h) {

    sceneManager->AddObject(std::make_shared<Camera>(0, 0, 800, 600));
    workSceneRenderer->updateSize(sceneManager->getCamera().get(), w, h);

    TextureManager::Init();

    // Add UI Buttons
    workSceneUI->addButton(std::make_shared<UIButton>(this, 5, translation_png, translation_png_len, [this]() {
        this->setMode(TRANSLATION_MODE);
    }, TRANSLATION_MODE));

    workSceneUI->addButton(std::make_shared<UIButton>(this, 5, free_camera_png, free_camera_png_len, [this]() {
        this->setMode(FREE_CAMERA_MODE);
    }, FREE_CAMERA_MODE));

    this->setMode(FREE_CAMERA_MODE);
    updateActiveButton();

    crossCursor = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
}

WorkSceneController::~WorkSceneController() {
    glfwDestroyCursor(crossCursor);
}

void WorkSceneController::update(GLFWwindow* window, float mouseWheel, float deltaTime, bool isMouseOverPanel) {
    processKeyboardInput(window, deltaTime);
    processMouseInput(window, mouseWheel, deltaTime, isMouseOverPanel);
}

void WorkSceneController::render(int x, int y, int w, int h) {
    workSceneRenderer->updateSize(sceneManager->getCamera().get(), w, h);
    posX = x;
    posY = y;
    width = w;
    height = h;
    workSceneRenderer->render(sceneManager->getCamera().get(), sceneManager.get());
    workSceneUI->render(shader.get()->getProgram(), width, height);
    workSceneRenderer->postRender();
}

void WorkSceneController::processKeyboardInput(GLFWwindow* window, float deltaTime) {
    
}

void WorkSceneController::processMouseInput(GLFWwindow* window, float mouseWheel, float deltaTime, bool isMouseOverPanel) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glm::vec2 mousePos(mouseX - posX, mouseY - posY);
    
    SceneCamera* camera = this->sceneManager->getCamera().get();

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        if (!isMouseDragging) {
            if (!isMouseOverPanel) return;
            isMouseDragging = true;
            lastMouseX = mousePos.x; 
            lastMouseY = mousePos.y;
        } else {
            glfwSetCursor(window, crossCursor);

            // Calculate the mouse movement
            double deltaX = mousePos.x - lastMouseX;
            double deltaY = mousePos.y - lastMouseY;

            // Adjust the camera based on mouse movement
            this->sceneManager->getCamera().get()->Move(glm::vec2(static_cast<float>(-deltaX / camera->GetZoom()), static_cast<float>(deltaY / camera->GetZoom())));

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
    mousePos.y = height - mousePos.y;

    // UI Buttons
    auto uiButtons = workSceneUI->getButtons();
    if (uiButtons.size() > 0 && (!isMouseDragging && !isMouseSelecting)) {
        for (const auto& button : uiButtons) {
            if (button->isMouseOver(mousePos)) {
                button->setHovered(true);
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                    button->setPressed(true);
                } else if (button->isPressed()) {
                    button->setPressed(false);
                    button->onClick();
                }
                return;
            } else {
                button->setHovered(false);
                button->setPressed(false);
            }
        }
    }

    // Select objects logic
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isMouseDragging) {
        // Check if mouse in work scene
        if (!isMouseSelecting && (mousePos.x < 0 || mousePos.x > width || mousePos.y < 0 || mousePos.y > height)) {
            if (!isMouseOverPanel) return;
            isMouseSelectBlocked = true;
            return;
        }

        if (isMouseSelectBlocked)
            return;

        if (!isMouseSelecting) {
            if (!isMouseOverPanel) return;
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
            bool isUniquePoint = ((int) finalMouseX == (int)lastMouseX) && ((int) finalMouseY == (int)lastMouseY);
            if (isUniquePoint) {
                filteredObjects = sceneManager->GetObjectsInCoords(glm::vec2(finalMouseX, finalMouseY));
            } else {
                filteredObjects = sceneManager->GetObjectsInCoords(glm::vec4(lastMouseX, lastMouseY, finalMouseX, finalMouseY));
            }

            auto& activeObjects = sceneManager->GetActiveObjects();

            // If shift pressed don't clear
            bool isShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

            if (!isShiftPressed)
                activeObjects.clear();

            if (filteredObjects->size() == 0) {
                // No objects found
            } else if (filteredObjects->size() == 1) {
                if (sceneManager->IsActiveObject(filteredObjects->at(0))) {
                    sceneManager->RemoveActiveObject(filteredObjects->at(0));
                } else {
                    sceneManager->AddActiveObject(filteredObjects->at(0));
                }
                lastSelectedObjectIndex = -1;
            } else {
                if (isUniquePoint) {
                    if (isShiftPressed) {
                        for (const auto& object : *filteredObjects) {
                            if (sceneManager->IsActiveObject(object)) {
                                sceneManager->RemoveActiveObject(object);
                            } else {
                                sceneManager->AddActiveObject(object);
                            }
                        }
                        lastSelectedObjectIndex = -1;
                    } else {
                        lastSelectedObjectIndex = (lastSelectedObjectIndex + 1) % filteredObjects->size();
                        sceneManager->AddActiveObject(filteredObjects->at(lastSelectedObjectIndex));
                    }
                } else {
                    for (const auto& object : *filteredObjects) {
                        if (sceneManager->IsActiveObject(object)) {
                            sceneManager->RemoveActiveObject(object);
                        } else {
                            sceneManager->AddActiveObject(object);
                        }
                    }
                    lastSelectedObjectIndex = -1;
                }
            }

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
            camera->SetPosition(cameraPos);
            camera->SetZoom(newZoom);
        }
    }
}

void WorkSceneController::setMode(WorkSceneMode mode) {
    if (currentMode != mode) {
        currentMode = mode;
        updateActiveButton();
    }
}

void WorkSceneController::updateActiveButton() {
    auto uiButtons = workSceneUI->getButtons();

    for (const auto& button : uiButtons) {
        button->setActive(button->getMode() == currentMode);
    }
}

GLuint WorkSceneController::getTexture() {
    return workSceneRenderer->getTexture();
}

