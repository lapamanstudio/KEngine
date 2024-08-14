#include "graphics/scene/objects/components/CameraComponent.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/Colors.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void CameraComponent::Render(GLuint shaderProgram) {
    if (auto parent = parentObject.lock()) { 
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(parent->GetPosition(), 0.0f));
        model = glm::translate(model, glm::vec3(parent->GetSize() * 0.5f, 0.0f));
        model = glm::rotate(model, glm::radians(parent->GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-parent->GetSize() * 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(parent->GetSize(), 1.0f));

        GLHelper::setModelMatrix(model);
        GLHelper::setColor3f(Colors::White);

        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }
}
