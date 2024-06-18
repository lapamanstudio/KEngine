#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

// 2D Camera
class SceneCamera {
public:
    SceneCamera();
    ~SceneCamera();
    
    void SetPosition(const glm::vec2& position);
    void SetZoom(float zoom);
    
    const glm::vec2& GetPosition() const;
    float GetZoom() const;
    
    void Move(const glm::vec2& offset);
    void Zoom(float factor);

    glm::mat4 GetProjection();
    void SetProjection(glm::mat4 projection);
    
    glm::mat4 GetViewMatrix() const;
    glm::vec2 screenToWorld(const glm::vec2& screenCoords, const glm::vec2& screenSize) const;

    float GetViewWidth() const;
    float GetViewHeight() const;
private:
    glm::mat4 projection;

    glm::vec2 position;
    float rotation;
    float zoom;
};

#endif // SCENE_CAMERA_H
