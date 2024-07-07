#ifndef CAMERA_H
#define CAMERA_H

#include "graphics/scene/objects/GameObject.h"
#include "graphics/fonts/IconsFontAwesome5.h"

class Camera : public GameObject {
public:
    Camera(float x, float y, float width, float height) : GameObject(ICON_FA_CAMERA_RETRO " Camera", x, y, width, height) {}

    void Update(float deltaTime) override {};
    void Render(GLuint shaderProgram) override;
};

#endif // CAMERA_H
