#ifndef OBJECT_COMPONENT_H
#define OBJECT_COMPONENT_H

#include "editor/graphics/scene/property/ObjectProperty.h"
#include "editor/graphics/scene/objects/IObject.h"
#include "editor/graphics/fonts/IconsFontAwesome5.h"
#include "editor/graphics/scene/objects/components/ObjectComponentFactory.h"

#include <GL/glew.h>
#include <memory>
#include <string>

class ObjectComponent {
public:
    ObjectComponent(const std::string& name, std::weak_ptr<IObject> parent)
        : parentObject(parent), properties(name, true, [this, name]() {
            if (auto parent = parentObject.lock()) {
                parent->RemoveComponent(name);
            }
        }), name(name) {}

    virtual void Render(GLuint shaderProgram) = 0;

    void RenderProperties() {
        properties.Render();
    }

    nlohmann::json Serialize() const {
        nlohmann::json j;
        j["name"] = name;
        j["properties"] = properties.Serialize();
        return j;
    }

    static std::shared_ptr<ObjectComponent> Deserialize(const std::weak_ptr<IObject> parent, const nlohmann::json& j) {
        std::string componentName;
        if (j.contains("name")) {
            componentName = j["name"];
        }

        printf("Deserializing component: %s\n", componentName.c_str());
        std::shared_ptr<ObjectComponent> component = ObjectComponentFactory::CreateComponent(componentName, parent);
        if (component && j.contains("properties")) {
            component->properties.Deserialize(j["properties"]);
        }
        return component;
    }

    virtual std::string GetTypeIcon() const { return ICON_FA_H_SQUARE; }    
    std::string GetName() const { return name; }

protected:
    std::weak_ptr<IObject> parentObject;
    GroupProperty properties;

private:
    std::string name;
};

#endif // OBJECT_COMPONENT_H
