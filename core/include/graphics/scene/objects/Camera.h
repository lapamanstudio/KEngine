#ifndef CAMERA_H
#define CAMERA_H

#include "graphics/scene/objects/GameObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

class Camera : public GameObject {
public:
    Camera(float x, float y, float width, float height, float zoom = 1.0f)
        : GameObject(x, y, width, height), zoom(zoom) {}

    void Update(float deltaTime) override;
    void Render(GLuint shaderProgram) override;

    void Move(const glm::vec2& delta) {
        position += delta;
    }

    void SetZoom(float newZoom) {
        zoom = newZoom;
    }

    void ObjectName() override {
        return "Camera";
    }

    glm::mat4 GetViewMatrix() const {
        return glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));
    }

private:
    float zoom;
};

#endif // CAMERA_H
