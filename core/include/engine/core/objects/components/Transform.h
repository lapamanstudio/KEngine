#pragma once

#include "engine/core/math/Vector2.h"

class Transform {
public:
    Transform();
    ~Transform();

    void SetPosition(float x, float y);
    Vector2 GetPosition() const;

    static void Internal_GetPosition(void* nativeTransformPtr, Vector2* outPosition);
    static void Internal_SetPosition(void* nativeTransformPtr, Vector2* position);
private:
    Vector2 position;
};
