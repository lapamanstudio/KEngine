#pragma once
#include "engine/core/objects/components/Vector2.h"

class Transform {
public:
    Transform();
    ~Transform();

    void SetPosition(float x, float y);
    Vector2 GetPosition() const;

private:
    Vector2 position;
};
