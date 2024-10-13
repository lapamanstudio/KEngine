#include "engine/core/objects/components/CameraViewComponent.h"

#include "engine/graphics/WindowFactory.h"

CameraViewComponent::CameraViewComponent(float viewportWidth, float viewportHeight)
    : x(0), y(0), zoom(1.0f), viewportWidth(viewportWidth), viewportHeight(viewportHeight) {
    projectionMatrix = glm::ortho(0.0f, viewportWidth, 0.0f, viewportHeight, -1.0f, 1.0f);
}

CameraViewComponent::~CameraViewComponent() {}

const glm::vec2& CameraViewComponent::GetPosition() const {
    return glm::vec2(x, y);
}

float& CameraViewComponent::GetZoom() {
    return zoom;
}

void CameraViewComponent::SetPosition(const glm::vec2& position) {
    this->x = position.x;
    this->y = position.y;
}

void CameraViewComponent::SetZoom(float zoom) {
    this->zoom = zoom;
}

void CameraViewComponent::Move(const glm::vec2& offset) {
    x += offset.x;
    y += offset.y;
}

void CameraViewComponent::Zoom(float factor) {
    zoom += factor;
}

glm::mat4 CameraViewComponent::GetProjectionMatrix() const {
    return projectionMatrix;
}

void CameraViewComponent::SetProjectionMatrix(glm::mat4 projection) {
    this->projectionMatrix = projection;
}

glm::mat4 CameraViewComponent::GetViewMatrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));
    view = glm::translate(view, -glm::vec3(x, y, 0.0f));
    return view;
}

void CameraViewComponent::OnResize(int windowWidth, int windowHeight) {
    float targetAspect = viewportWidth / viewportHeight;
    float windowAspect = static_cast<float>(windowWidth) / windowHeight;

    int vpWidth, vpHeight;
    int vpX, vpY;

    if (windowAspect >= targetAspect) {
        vpHeight = windowHeight;
        vpWidth = static_cast<int>(vpHeight * targetAspect);
        vpX = (windowWidth - vpWidth) / 2;
        vpY = 0;
    } else {
        vpWidth = windowWidth;
        vpHeight = static_cast<int>(vpWidth / targetAspect);
        vpX = 0;
        vpY = (windowHeight - vpHeight) / 2;
    }

    glViewport(vpX, vpY, vpWidth, vpHeight);
}
