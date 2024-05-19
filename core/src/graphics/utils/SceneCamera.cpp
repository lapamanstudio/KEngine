#include "graphics/utils/SceneCamera.h"

SceneCamera::SceneCamera() {
    position = { 0.0f, 0.0f };
    rotation = 0.0f;
    zoom = 1.0f;
}

SceneCamera::~SceneCamera() {
    
}

const glm::vec2& SceneCamera::GetPosition() const {
    return position;
}

float SceneCamera::GetRotation() const {
    return rotation;
}

float SceneCamera::GetZoom() const {
    return zoom;
}

void SceneCamera::SetPosition(const glm::vec2& position) {
    this->position = position;
}

void SceneCamera::SetRotation(float rotation) {
    this->rotation = rotation;
}

void SceneCamera::SetZoom(float zoom) {
    this->zoom = zoom;
}

void SceneCamera::Move(const glm::vec2& offset) {
    position += offset;
}

void SceneCamera::Rotate(float offset) {
    rotation += offset;
}

void SceneCamera::Zoom(float factor) {
    zoom *= factor;
}

glm::mat4 SceneCamera::GetViewMatrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(position, 0.0f));
    view = glm::rotate(view, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));
    return view;
}
