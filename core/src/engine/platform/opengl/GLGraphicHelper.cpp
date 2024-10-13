#include "engine/platform/opengl/GLGraphicHelper.h"
#include "engine/platform/opengl/GLShaderHelper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GLGraphicHelper::GLGraphicHelper() {
    const GLchar* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
        }
    )";

    const GLchar* fragmentShaderSource = R"(
        #version 330 core

        out vec4 FragColor;

        uniform vec4 vertexColor;

        void main()
        {
            FragColor = vertexColor;
        }
    )";

    // Load shaders
    defaultShader = GLShaderHelper::LoadShaders(vertexShaderSource, fragmentShaderSource);
    if (!defaultShader) {
        std::cerr << "Failed to create shader program" << std::endl;
        return;
    }

    currentShaderProgram = defaultShader->GetProgramID();

    // Cache uniform locations
    modelLoc = glGetUniformLocation(currentShaderProgram, "model");
    viewLoc = glGetUniformLocation(currentShaderProgram, "view");
    projLoc = glGetUniformLocation(currentShaderProgram, "projection");
    colorLoc = glGetUniformLocation(currentShaderProgram, "vertexColor");

    // Prepare Rect VAO and VBO
    GLfloat vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

GLGraphicHelper::~GLGraphicHelper() {
    delete defaultShader;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void GLGraphicHelper::SetMat4(GLint location, const glm::mat4& mat) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void GLGraphicHelper::SetColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    glUniform4f(colorLoc, r, g, b, a);
}

void GLGraphicHelper::SetViewMatrix(const glm::mat4& view) {
    SetMat4(viewLoc, view);
}

void GLGraphicHelper::SetProjectionMatrix(const glm::mat4& projection) {
    SetMat4(projLoc, projection);
}

void GLGraphicHelper::SetModelMatrix(const glm::mat4& model) {
    SetMat4(modelLoc, model);
}

void GLGraphicHelper::UseDefaultShader() {
    defaultShader->Use();
    currentShaderProgram = defaultShader->GetProgramID();
}

void GLGraphicHelper::FillRect(float x, float y, float width, float height) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    SetMat4(modelLoc, model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}
