#include "graphics/scene/objects/components/SpriteRendererComponent.h"
#include "graphics/scene/objects/EmptyObject.h"
#include "graphics/utils/TextureManager.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/Colors.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void SpriteRendererComponent::Render(GLuint shaderProgram) {
    if (textureID == 0) onChangeSpriteName();
    if (auto parent = parentObject.lock()) { 
        glm::mat4 model = glm::mat4(1.0f);
        
        model = glm::translate(model, glm::vec3(parent->GetPosition(), 0.0f));
        model = glm::rotate(model, glm::radians(parent->GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(parent->GetSize() * parent->GetScale(), 1.0f));

        GLHelper::setModelMatrix(model);
        GLHelper::setColor3f(Colors::White);

        if (textureID == 0) {
            GLHelper::setColor3f(Colors::Red);
            parent->DrawRectangle();
            return;
        }
        TextureManager::renderTexture(textureID, 0, 0, 1, 1);
        GLHelper::disableTexture();
    }
}
