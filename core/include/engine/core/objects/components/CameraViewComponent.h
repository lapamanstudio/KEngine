#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraViewComponent {
public:
    CameraViewComponent(float width, float height);
    ~CameraViewComponent();
    
    void SetPosition(const glm::vec2& position);
    void SetZoom(float zoom);

    const glm::vec2& GetPosition() const;
    float& GetZoom();

    void Move(const glm::vec2& offset);
    void Zoom(float factor);    

    void SetProjectionMatrix(glm::mat4 projection);
    
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;

    void OnResize(int windowWidth, int windowHeight);

    float GetViewportWidth() const { return viewportWidth; }
    float GetViewportHeight() const { return viewportHeight; }

    bool isMainCamera = false;

private:
    float x, y;
    float zoom;
    float viewportWidth, viewportHeight;
    
    glm::mat4 projectionMatrix;
};

#endif // CAMERA_H
