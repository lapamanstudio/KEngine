#ifndef OBJECT_COMPONENT_H
#define OBJECT_COMPONENT_H

#include "graphics/scene/property/ObjectProperty.h"
#include "graphics/scene/objects/IObject.h"
#include "graphics/fonts/IconsFontAwesome5.h"

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

    virtual std::string GetTypeIcon() const { return ICON_FA_H_SQUARE; }    
    std::string GetName() const { return name; }

protected:
    std::weak_ptr<IObject> parentObject;
    GroupProperty properties;

private:
    std::string name;
};

#endif // OBJECT_COMPONENT_H
