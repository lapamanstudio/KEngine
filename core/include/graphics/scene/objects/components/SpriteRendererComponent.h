#ifndef SPRITE_RENDERER_COMPONENT_H
#define SPRITE_RENDERER_COMPONENT_H

#include "graphics/scene/objects/components/ObjectComponent.h"
#include "graphics/utils/TextureManager.h"

#include <glm/vec2.hpp>

class SpriteRendererComponent : public ObjectComponent {
public:
    SpriteRendererComponent(std::weak_ptr<IObject> parent) : ObjectComponent("Sprite Renderer", parent), spriteName(std::string()), textureID(0) {
        properties.AddProperty(std::make_shared<StringProperty>("Sprite path", &spriteName, [this]() { onChangeSpriteName(); }));
    }

    void Render(GLuint shaderProgram) override;

    std::string GetTypeIcon() const override { return ICON_FA_IMAGE; }

    std::string GetSpriteName() const { return spriteName; }
    void SetSpriteName(const std::string& name) { spriteName = name; onChangeSpriteName(); }
private:
    void onChangeSpriteName() {
        textureID = spriteName.empty() ? 0 : TextureManager::LoadTextureAsset(spriteName.c_str());
    }

    std::string spriteName;
    GLuint textureID;
};

#endif // SPRITE_RENDERER_COMPONENT_H
