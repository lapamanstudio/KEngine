#include "graphics/utils/Camera.h"

Camera::Camera() {
    position = { 0.0f, 0.0f };
    rotation = 0.0f;
    zoom = 1.0f; // Default zoom factor
}

Camera::~Camera() {
    // Empty
}

const glm::vec2& Camera::GetPosition() const {
    return position;
}

float Camera::GetRotation() const {
    return rotation;
}

float Camera::GetZoom() const {
    return zoom;
}

void Camera::SetPosition(const glm::vec2& position) {
    this->position = position;
}

void Camera::SetRotation(float rotation) {
    this->rotation = rotation;
}

void Camera::SetZoom(float zoom) {
    this->zoom = zoom;
}

void Camera::Move(const glm::vec2& offset) {
    position += offset;
}

void Camera::Rotate(float offset) {
    rotation += offset;
}

void Camera::Zoom(float factor) {
    zoom *= factor;
}

glm::mat4 Camera::GetViewMatrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(position, 0.0f));
    view = glm::rotate(view, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));
    return view;
}
