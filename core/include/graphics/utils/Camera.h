#ifndef CAMERA_H
#define CAMERA_H

#include "linmath.h"

// 2D Camera
class Camera {
public:
    Camera();
    ~Camera();
    
    void SetPosition(const vec2& position);
    void SetRotation(float rotation);
    
    const vec2& GetPosition() const;
    float GetRotation() const;
    
    void Move(const vec2& offset);
    void Rotate(float offset);
    
    mat4 GetViewMatrix() const;
private:
    vec2 position;
    float rotation;
};

#endif // CAMERA_H
