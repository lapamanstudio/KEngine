#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "graphics/scene/objects/components/ObjectComponent.h"

#include <glm/vec2.hpp>

class CameraComponent : public ObjectComponent {
public:
    CameraComponent(std::weak_ptr<IObject> parent) : ObjectComponent("Camera", parent) {
        static float fov = 1.0f;
        // TODO Remove this property or implement the FOV, but actually is not used
        properties.AddProperty(std::make_shared<FloatProperty>("FOV", &fov, 0.1f));
    }

    void Render(GLuint shaderProgram) override;

    std::string GetTypeIcon() const override { return ICON_FA_CAMERA_RETRO; }
};

#endif // CAMERA_COMPONENT_H
