#include "engine/core/objects/components/Transform.h"
#include "engine/core/objects/components/Script.h"

Transform::Transform() : position(0.0f, 0.0f) {}

Transform::~Transform() {}

void Transform::SetPosition(float x, float y) {
    position.x = x;
    position.y = y;
}

Vector2 Transform::GetPosition() const {
    return position;
}