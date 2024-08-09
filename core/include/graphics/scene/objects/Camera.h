#ifndef CAMERA_H
#define CAMERA_H

#include "graphics/scene/objects/GameObject.h"
#include "graphics/fonts/IconsFontAwesome5.h"

class Camera : public GameObject {
public:
    Camera(float x, float y, float width, float height) : GameObject("Camera", x, y, width, height) {}

    void Update(float deltaTime) override {};
    void Render(GLuint shaderProgram) override;
    std::shared_ptr<GameObject> Clone() const override { return std::make_shared<Camera>(*this); }

    std::string GetTypeIcon() const override { return ICON_FA_CAMERA_RETRO; }
};

#endif // CAMERA_H
