#ifndef EMPTYOBJECT_H
#define EMPTYOBJECT_H

#include "graphics/scene/objects/components/ObjectComponent.h"
#include "graphics/scene/objects/IObject.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/Colors.h"
#include "graphics/math/MathUtil.h"
#include "graphics/fonts/IconsFontAwesome5.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <unordered_map>
#include <vector>
#include <memory>

class EmptyObject : public IObject {
public:
    EmptyObject(float x, float y, float width, float height, const std::string& name = "Empty Object")
        : name(name), position(x, y), size(width, height), scale(1, 1), rotation(0.0f), components() {

        InitRenderData();

        this->name.resize(32, '\0');
        LinkProperties();
    }

    void CleanAndLinkProperties() {
        properties.Clear();
        LinkProperties();
    }

    void LinkProperties() {
        properties.AddProperty(std::make_shared<StringProperty>("Name", &name));

        auto transformGroup = std::make_shared<GroupProperty>("Transform");
        transformGroup->AddProperty(std::make_shared<Vec2FloatProperty>("Position", &position.x, &position.y, 1.0f));
        transformGroup->AddProperty(std::make_shared<Vec2FloatProperty>("Size", &size.x, &size.y, 0.1f, 0.0f));
        transformGroup->AddProperty(std::make_shared<Vec2FloatProperty>("Scale", &scale.x, &scale.y, 0.001f, 0.0f));
        transformGroup->AddProperty(std::make_shared<FloatProperty>("Rotation", &rotation, 0.1f));

        properties.AddProperty(transformGroup);
    }

    virtual ~EmptyObject() {}

    void Render(GLuint shaderProgram) {
        if (components.size() == 0) {
            GLHelper::setColor3f(Colors::White);
            RenderHighlightedBox(shaderProgram);
        }

        for (auto& component : components) {
            component->Render(shaderProgram);
        }
    }

    virtual std::shared_ptr<EmptyObject> Clone() {
        auto newObject = std::make_shared<EmptyObject>(*this);
        newObject->CleanAndLinkProperties();
        return newObject;
    }

    glm::vec2 GetPosition() const override { return position; }
    void SetPosition(const glm::vec2& pos) override { position = pos; }

    void Move(const glm::vec2& delta) { position += delta; }

    glm::vec2 GetSize() const override { return size; }
    void SetSize(const glm::vec2& sz) override { size = sz; }

    glm::vec2 GetScale() const override { return scale; }
    void SetScale(const glm::vec2& sc) override { scale = sc; }

    float GetRotation() const override { return rotation; }
    void SetRotation(float rot) override { rotation = rot; }

    std::string GetName() const override { return name; }
    virtual std::string GetTypeIcon() const {  return components.size() == 0 ? ICON_FA_SQUARE : components[0]->GetTypeIcon(); }

    bool IsInCoords(const glm::vec2& coords) const {
        return MathUtil::IsPointInRect(coords, position, GetSize() * GetScale(), rotation);
    }

    void RenderHighlightedBox(GLuint shaderProgram) {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(GetPosition(), 0.0f));
        model = glm::rotate(model, glm::radians(GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(GetSize() * GetScale(), 1.0f));

        GLHelper::setModelMatrix(model);

        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    void RenderSelectedBox(GLuint shaderProgram) {
        GLHelper::setColor3f(Colors::Yellow);
        RenderHighlightedBox(shaderProgram);
    }

    const Properties& GetProperties() const {
        return properties;
    }

    void RenderProperties() const {
        properties.Render();
    }

    static GLuint GetVAO() { return VAO; }
    static GLuint GetVBO() { return VBO; }

    void AddComponent(std::shared_ptr<ObjectComponent> component) {
        components.push_back(component);
    }

protected:
    std::string name;
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 scale;
    float rotation;

    static GLuint VAO;
    static GLuint VBO;

    void InitRenderData() {
        if (VAO == 0) {
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
    }

private:
    Properties properties;
    std::vector<std::shared_ptr<ObjectComponent>> components;
};

#endif // EMPTYOBJECT_H
