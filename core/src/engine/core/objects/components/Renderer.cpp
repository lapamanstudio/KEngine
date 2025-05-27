#include "engine/core/objects/components/Renderer.h"
#include "engine/core/renderer/GraphicHelper.h"
#include "engine/core/renderer/TextureManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::Render(Transform* transform) {
    glm::mat4 model = glm::mat4(1.0f);
    
    auto pos = transform->GetPosition();
    model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
    // model = glm::rotate(model, glm::radians(transform->GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
    // model = glm::scale(model, glm::vec3(transform->GetSize() * transform->GetScale(), 1.0f));

    GraphicHelper::GetInstance()->SetModelMatrix(model);
    GraphicHelper::GetInstance()->SetColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Set color to white

    // TextureManager::RenderTexture(textureID == -1 ? -1 : textureID, 0, 0, 1, 1);
}
