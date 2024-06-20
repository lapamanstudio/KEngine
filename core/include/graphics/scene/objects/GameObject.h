#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/Colors.h"
#include "graphics/scene/property/ObjectProperty.h"
#include "graphics/math/MathUtil.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <unordered_map>
#include <vector>
#include <memory>

class GameObject {
public:
    GameObject(float x, float y, float width, float height)
        : position(x, y), size(width, height), rotation(0.0f) {
        InitRenderData();
    }

    virtual ~GameObject() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    virtual void Update(float deltaTime) = 0;
    virtual void Render(GLuint shaderProgram) = 0;
    virtual const char* GetObjectName() const = 0;

    glm::vec2 GetPosition() const { return position; }
    void SetPosition(const glm::vec2& pos) { position = pos; }

    void Move(const glm::vec2& delta) {
        position += delta;
    }

    glm::vec2 GetSize() const { return size; }
    void SetSize(const glm::vec2& sz) { size = sz; }

    float GetRotation() const { return rotation; }
    void SetRotation(float rot) { rotation = rot; }

    bool IsInCoords(const glm::vec2& coords) const {
        return MathUtil::IsPointInRect(coords, position, size);
    }

    // Yellow selection box around the object
    void RenderSelectionBox(GLuint shaderProgram) {
        GLHelper::useShader(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));
        model = glm::translate(model, glm::vec3(size * 0.5f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-size * 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(size, 1.0f));

        GLHelper::setModelMatrix(model);
        GLHelper::setColor3f(Colors::Yellow);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);

        GLHelper::unuseShader();
    }

    void AddProperty(std::shared_ptr<Property> property) {
        properties[property.get()->GetName()] = property;
    }

    const std::unordered_map<std::string, std::shared_ptr<Property>>& GetProperties() const {
        return properties;
    }

protected:
    glm::vec2 position;
    glm::vec2 size;
    float rotation;
    GLuint VAO, VBO;

    void InitRenderData() {
        static const float vertices[] = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
private:
    std::unordered_map<std::string, std::shared_ptr<Property>> properties;
};

#endif // GAMEOBJECT_H
