#ifndef SPRITE_RENDERER_COMPONENT_H
#define SPRITE_RENDERER_COMPONENT_H

#include "graphics/scene/objects/components/ObjectComponent.h"

#include <glm/vec2.hpp>

class SpriteRendererComponent : public ObjectComponent {
public:
    SpriteRendererComponent(std::string spriteName, std::weak_ptr<IObject> parent) : ObjectComponent("Sprite Renderer", parent), spriteName(spriteName) {
        properties.AddProperty(std::make_shared<StringProperty>("Sprite", &spriteName));
    }

    void Render(GLuint shaderProgram) override;

    std::string GetTypeIcon() const override { return ICON_FA_IMAGE; }

    std::string GetSpriteName() const { return spriteName; }
    void SetSpriteName(const std::string& name) { spriteName = name; }
private:
    std::string spriteName;
};

#endif // SPRITE_RENDERER_COMPONENT_H