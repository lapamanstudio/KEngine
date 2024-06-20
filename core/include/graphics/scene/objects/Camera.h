#ifndef CAMERA_H
#define CAMERA_H

#include "graphics/scene/objects/GameObject.h"

class Camera : public GameObject {
public:
    Camera(float x, float y, float width, float height) : GameObject(x, y, width, height) {
        AddProperty(std::make_shared<FloatProperty>("PositionX", &position.x));
    }

    void Update(float deltaTime) override {};
    void Render(GLuint shaderProgram) override;

    const char* GetObjectName() const override {
        return "Camera";
    }
};

#endif // CAMERA_H
