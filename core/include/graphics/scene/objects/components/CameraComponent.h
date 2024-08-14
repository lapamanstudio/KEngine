#ifndef SPRITE_RENDERER_COMPONENT_H
#define SPRITE_RENDERER_COMPONENT_H

#include "graphics/scene/objects/components/ObjectComponent.h"

#include <glm/vec2.hpp>

class CameraComponent : public ObjectComponent {
public:
    CameraComponent(std::weak_ptr<IObject> parent) : ObjectComponent("Camera", parent) {
        static float fov = 1.0f;
        properties.AddProperty(std::make_shared<FloatProperty>("FOV", &fov, 0.1f));
    }

    void Render(GLuint shaderProgram) override;

    std::string GetTypeIcon() const override { return ICON_FA_CAMERA_RETRO; }
};

#endif // SPRITE_RENDERER_COMPONENT_H