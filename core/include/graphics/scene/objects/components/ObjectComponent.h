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
    ObjectComponent(std::string name, std::weak_ptr<IObject> parent) 
        : parentObject(parent), name(name)  {}

    virtual void Render(GLuint shaderProgram) = 0;

    virtual std::string GetTypeIcon() const { return ICON_FA_H_SQUARE; }

protected:
    std::weak_ptr<IObject> parentObject;
    Properties properties;

private:
    std::string name;
};

#endif // OBJECT_COMPONENT_H
