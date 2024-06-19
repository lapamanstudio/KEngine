#include "graphics/scene/SceneCamera.h"
#include <cstdio>

SceneCamera::SceneCamera() {
    position = { 0.0f, 0.0f };
    zoom = 0.7f;
}

SceneCamera::~SceneCamera() {}

const glm::vec2& SceneCamera::GetPosition() const {
    return position;
}

float SceneCamera::GetZoom() const {
    return zoom;
}

void SceneCamera::SetPosition(const glm::vec2& position) {
    this->position = position;
}

void SceneCamera::SetZoom(float zoom) {
    this->zoom = zoom;
}

void SceneCamera::Move(const glm::vec2& offset) {
    position += offset;
}

void SceneCamera::Zoom(float factor) {
    zoom += factor;
}

glm::mat4 SceneCamera::GetProjection() {
    return projection;
}

void SceneCamera::SetProjection(glm::mat4 projection) {
    this->projection = projection;
}

glm::mat4 SceneCamera::GetViewMatrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::scale(view, glm::vec3(1.0f * zoom, 1.0f * zoom, 1.0f));
    view = glm::translate(view, -glm::vec3(position, 0.0f));
    return view;
}

glm::vec2 SceneCamera::screenToWorld(const glm::vec2& screenCoords, const glm::vec2& screenSize) const {
    // Normalize screen coordinates to range [-1, 1]
    glm::vec2 normalizedScreen = glm::vec2(
        (screenCoords.x / (screenSize.x + 8)) * 2 - 1, // 0 - LEFT | 1 - RIGHT
        1 - (screenCoords.y / (screenSize.y + 13)) * 2  // 1 - TOP | 0 - BOTTOM
    );

    // Convert normalized screen coordinates to homogeneous coordinates
    glm::vec4 screenCoordsH = glm::vec4(normalizedScreen, 0.0f, 1.0f);

    // Get the view and projection matrices
    glm::mat4 viewMatrix = GetViewMatrix();
    glm::mat4 inverseVPMatrix = glm::inverse(projection * viewMatrix);

    // Transform to world coordinates
    glm::vec4 worldCoordsH = inverseVPMatrix * screenCoordsH;
    glm::vec2 worldCoords = glm::vec2(worldCoordsH);

    return worldCoords;
}

float SceneCamera::GetViewWidth() const {
    glm::vec4 leftBottom = glm::inverse(projection * GetViewMatrix()) * glm::vec4(-1, -1, 0, 1);
    glm::vec4 rightBottom = glm::inverse(projection * GetViewMatrix()) * glm::vec4(1, -1, 0, 1);
    return rightBottom.x - leftBottom.x;
}

float SceneCamera::GetViewHeight() const {
    glm::vec4 leftBottom = glm::inverse(projection * GetViewMatrix()) * glm::vec4(-1, -1, 0, 1);
    glm::vec4 leftTop = glm::inverse(projection * GetViewMatrix()) * glm::vec4(-1, 1, 0, 1);
    return leftBottom.y - leftTop.y;
}
