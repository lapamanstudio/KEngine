#include "graphics/scene/objects/Camera.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/Colors.h"

#include <glm/gtc/type_ptr.hpp>

void Camera::Render(GLuint shaderProgram) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(size * 0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-size * 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    GLHelper::setModelMatrix(model);
    GLHelper::setColor3f(Colors::White);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
}
