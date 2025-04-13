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


// Internal API [KEngine.Components.Transform]
void Transform::Internal_GetPosition(void* nativeTransformPtr, Vector2* outPosition) {
    Transform* transform = (Transform*)nativeTransformPtr;
    if (transform) {
        *outPosition = transform->GetPosition();
    }
}

void Transform::Internal_SetPosition(void* nativeTransformPtr, Vector2* position) {
    Transform* transform = (Transform*)nativeTransformPtr;
    if (transform) {
        transform->SetPosition(position->x, position->y);
    }
}
