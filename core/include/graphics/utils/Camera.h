#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

// 2D Camera
class Camera {
public:
    Camera();
    ~Camera();
    
    void SetPosition(const glm::vec2& position);
    void SetRotation(float rotation);
    void SetZoom(float zoom);
    
    const glm::vec2& GetPosition() const;
    float GetRotation() const;
    float GetZoom() const;
    
    void Move(const glm::vec2& offset);
    void Rotate(float offset);
    void Zoom(float factor);
    
    glm::mat4 GetViewMatrix() const;
private:
    glm::vec2 position;
    float rotation;
    float zoom;
};

#endif // CAMERA_H
