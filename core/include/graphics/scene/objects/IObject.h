#ifndef IOBJECT_H
#define IOBJECT_H

#include <glm/glm.hpp>
#include <string>

class IObject {
public:
    virtual ~IObject() = default;

    virtual glm::vec2 GetPosition() const = 0;
    virtual void SetPosition(const glm::vec2& pos) = 0;
    
    virtual glm::vec2 GetSize() const = 0;
    virtual void SetSize(const glm::vec2& sz) = 0;
    
    virtual float GetRotation() const = 0;
    virtual void SetRotation(float rot) = 0;

    virtual std::string GetName() const = 0;
};

#endif // IOBJECT_H
