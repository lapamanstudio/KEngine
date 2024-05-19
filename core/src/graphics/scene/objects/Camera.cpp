#include "graphics/scene/objects/Camera.h"
#include "graphics/drivers/GLHelper.h"

#include <glm/gtc/type_ptr.hpp>

void Camera::Render(GLuint shaderProgram) {
    GLHelper::useShader(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    GLHelper::setModelMatrix(model);
    GLHelper::setColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);

    GLHelper::unuseShader();
}
