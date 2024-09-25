#include "window/gui/panels/workscene/controller/WorkSceneController.h"
#include "window/gui/panels/workscene/gui/WorkSceneUI.h"
#include "graphics/drivers/ShaderHelper.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/TextureManager.h"

#include <imgui.h>

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

    // sceneManager->AddObject(std::make_shared<Camera>(0, 0, 800, 600));
    workSceneRenderer->updateSize(sceneManager->getCamera().get(), w, h);

    TextureManager::Init();

    // Add UI Buttons
    workSceneUI->addButton(std::make_shared<UIButton>(this, 5, "icons/translation.png", [this]() {
        this->setMode(TRANSLATION_MODE);
    }, TRANSLATION_MODE));

    workSceneUI->addButton(std::make_shared<UIButton>(this, 5, "icons/free_camera.png", [this]() {
        this->setMode(FREE_CAMERA_MODE);
    }, FREE_CAMERA_MODE));

    this->setMode(FREE_CAMERA_MODE);
    updateActiveButton(FREE_CAMERA_MODE);

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
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        bool ctrlPressed = ImGui::GetIO().KeyCtrl;

        if (ImGui::IsKeyPressed(ImGuiKey_G, false)) {
            setMode(TRANSLATION_MODE);
        } else if (ImGui::IsKeyPressed(ImGuiKey_F, false)) {
            setMode(FREE_CAMERA_MODE);
        } else if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
            sceneManager->SaveScene();
        } else if (ctrlPressed && ImGui::IsKeyPressed(ImGuiKey_R, false)) {
            sceneManager->LoadScene();
        }
    }
}

void WorkSceneController::processMouseInput(GLFWwindow* window, float mouseWheel, float deltaTime, bool isMouseOverPanel) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glm::vec2 mousePos(mouseX - posX, mouseY - posY);

    SceneCamera* camera = this->sceneManager->getCamera().get();

    // Mouse wrapping
    if (isMouseDragging && mousePos.x > width) {
        glfwSetCursorPos(window, posX, mouseY);
        mousePos.x = 0;
        mouseDirectionWhenMovingX = -1;
    } else if (isMouseDragging && mousePos.x < 0) {
        glfwSetCursorPos(window, posX + width, mouseY);
        mousePos.x = width;
        mouseDirectionWhenMovingX = 1;
    } else if (isMouseDragging && mousePos.y > height) {
        glfwSetCursorPos(window, mouseX, posY);
        mousePos.y = 0;
        mouseDirectionWhenMovingY = -1;
    } else if (isMouseDragging && mousePos.y < 0) {
        glfwSetCursorPos(window, mouseX, posY + height);
        mousePos.y = height;
        mouseDirectionWhenMovingY = 1;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        if (!isMouseDragging) {
            if (!isMouseOverPanel) return;
            isMouseDragging = true;
            lastMouseX = mousePos.x;
            lastMouseY = mousePos.y;
        } else {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

            lastMouseX = lastMouseX + (width * mouseDirectionWhenMovingX);
            lastMouseY = lastMouseY + (height * mouseDirectionWhenMovingY);

            // Calculate the mouse movement
            double deltaX = mousePos.x - lastMouseX;
            double deltaY = mousePos.y - lastMouseY;

            // Reset mouse direction
            mouseDirectionWhenMovingX = 0;
            mouseDirectionWhenMovingY = 0;

            // Adjust the camera based on mouse movement
            camera->Move(glm::vec2(static_cast<float>(-deltaX / camera->GetZoom()), static_cast<float>(deltaY / camera->GetZoom())));

            lastMouseX = mousePos.x;
            lastMouseY = mousePos.y;
        }
    } else {
        if (isMouseDragging) {
            isMouseDragging = false;
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

    // Scene camera modes
    if (sceneManager->GetMode() == WorkSceneMode::TRANSLATION_MODE) {
        std::optional<glm::vec2> middlePosition = sceneManager->GetMiddlePositionOfActiveObjects();
        glm::vec2 worldCoords = camera->screenToWorld(mousePos, glm::vec2(width, height));

        if (middlePosition.has_value()) {
            glm::vec2 middle = middlePosition.value();
            glm::vec2 centerRectPos = middle - glm::vec2(32, 32);
            glm::vec2 rightArrowRectPos = middle + glm::vec2(37, -30);
            glm::vec2 upperArrowRectPos = middle + glm::vec2(-37, 37);

            sceneManager->SetMouseInCenterCircle(MathUtil::IsPointInRect(worldCoords, centerRectPos, glm::vec2(64, 64)));
            sceneManager->SetMouseInRightArrow(MathUtil::IsPointInRect(worldCoords, rightArrowRectPos, glm::vec2(111, 60)));
            sceneManager->SetMouseInUpperArrow(MathUtil::IsPointInRect(worldCoords, upperArrowRectPos, glm::vec2(74, 101)));
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isMouseDragging && !isMouseSelecting) {
            static glm::vec2 initialMouseWorldCoords;
            bool freeCheck = !isMouseMovingObjectsFromCenter && !isMouseMovingObjectsFromRight && !isMouseMovingObjectsFromUpper;

            auto handleMovement = [&](bool& isMovingFlag, bool isMouseInArea, auto updatePosition) {
                if (isMouseInArea && freeCheck) {
                    isMovingFlag = true;
                    initialMouseWorldCoords = worldCoords;
                }

                if (isMovingFlag) {
                    glm::vec2 mouseDelta = worldCoords - initialMouseWorldCoords;
                    for (const auto& object : sceneManager->GetActiveObjects()) {
                        updatePosition(object, mouseDelta);
                    }
                    initialMouseWorldCoords = worldCoords;
                }
            };

            handleMovement(isMouseMovingObjectsFromCenter, sceneManager->IsMouseInCenterCircle(), [](const auto& object, const glm::vec2& delta) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
                object->SetPosition(object->GetPosition() + delta);
            });

            handleMovement(isMouseMovingObjectsFromRight, sceneManager->IsMouseInRightArrow(), [](const auto& object, const glm::vec2& delta) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                object->SetPosition(glm::vec2(object->GetPosition().x + delta.x, object->GetPosition().y));
            });

            handleMovement(isMouseMovingObjectsFromUpper, sceneManager->IsMouseInUpperArrow(), [](const auto& object, const glm::vec2& delta) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
                object->SetPosition(glm::vec2(object->GetPosition().x, object->GetPosition().y + delta.y));
            });

            if (!freeCheck) return;
        } else {
            // Reset flags on release
            isMouseMovingObjectsFromCenter = false;
            isMouseMovingObjectsFromUpper = false;
            isMouseMovingObjectsFromRight = false;
        }
    }

    // Select objects logic
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isMouseDragging && !isMouseMovingObjectsFromCenter && !isMouseMovingObjectsFromUpper && !isMouseMovingObjectsFromRight) {
        // Check if mouse in work scene
        if (!isMouseSelecting && !isMouseSelectBlocked) {
            if (!isMouseOverPanel) return;
            if (mousePos.x < 0 || mousePos.x > width || mousePos.y < 0 || mousePos.y > height) {
                isMouseSelectBlocked = true;
                return;
            }
            
            isMouseSelecting = true;

            glm::vec2 worldCoords = camera->screenToWorld(mousePos, glm::vec2(width, height));
            lastMouseX = worldCoords.x;
            lastMouseY = worldCoords.y;
        } else {
            glm::vec2 worldCoords = camera->screenToWorld(mousePos, glm::vec2(width, height));
            workSceneRenderer->setSelectionBox(glm::vec4(lastMouseX, lastMouseY, worldCoords.x, worldCoords.y));
        }
    } else {
        isMouseSelectBlocked = false;

        if (isMouseSelecting) {
            isMouseSelecting = false;

            // Get mouse coordinates in the camera
            glm::vec2 worldCoords = camera->screenToWorld(mousePos, glm::vec2(width, height));
            double finalMouseX = worldCoords.x;
            double finalMouseY = worldCoords.y;
            
            // Determine if selection is a single point or a rectangle
            bool isUniquePoint = (static_cast<int>(finalMouseX) == static_cast<int>(lastMouseX)) &&
                                (static_cast<int>(finalMouseY) == static_cast<int>(lastMouseY));

            // Get filtered objects based on selection
            std::unique_ptr<std::vector<std::shared_ptr<EmptyObject>>> filteredObjects = isUniquePoint
                ? sceneManager->GetObjectsInCoords(glm::vec2(finalMouseX, finalMouseY))
                : sceneManager->GetObjectsInCoords(glm::vec4(lastMouseX, lastMouseY, finalMouseX, finalMouseY));

            auto& activeObjects = sceneManager->GetActiveObjects();
            bool isShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

            if (!isShiftPressed)
                activeObjects.clear();

            if (!filteredObjects->empty()) {
                if (filteredObjects->size() == 1 || !isUniquePoint) {
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
            }

            workSceneRenderer->clearSelectionBox();
        }
    }

    // Zoom In/Out
    if (mouseWheel != 0) {
        float zoomFactor = mouseWheel * 0.1f;
        float newZoom = camera->GetZoom() + zoomFactor;

        if (newZoom > 0.1f && newZoom <= 10.0f) {
            glm::vec2 mousePosNormalize = (mousePos - glm::vec2(width / 2.0f, height / 2.0f)) / glm::vec2(width / 2.0f, height / 2.0f);
            glm::vec2 cameraPos = camera->GetPosition();
            glm::vec2 worldMousePos = cameraPos + mousePosNormalize * (width / (2.0f * camera->GetZoom()));

            glm::vec2 newCameraPos = worldMousePos - mousePosNormalize * (width / (2.0f * newZoom));
            camera->SetPosition(glm::mix(cameraPos, newCameraPos, 0.1f));
            camera->SetZoom(newZoom);
        }
    }
}

void WorkSceneController::setMode(WorkSceneMode mode) {
    if (sceneManager.get()->GetMode() != mode) {
        sceneManager.get()->SetMode(mode);
        updateActiveButton(mode);
    }
}

void WorkSceneController::updateActiveButton(WorkSceneMode mode) {
    auto uiButtons = workSceneUI->getButtons();

    for (const auto& button : uiButtons) {
        button->setActive(button->getMode() == mode);
    }
}

GLuint WorkSceneController::getTexture() {
    return workSceneRenderer->getTexture();
}
