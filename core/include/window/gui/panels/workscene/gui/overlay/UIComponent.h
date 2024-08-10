#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <functional>

class WorkSceneController;

class UIComponent {
public:
    UIComponent(WorkSceneController* controller, float radius);
    virtual ~UIComponent();

    virtual void render(int shaderProgram, float x, float y, float width, float height) = 0;
    virtual void onClick() = 0;
    virtual bool isMouseOver(glm::vec2 mousePos);

    void setHovered(bool hovered) {
        this->hovered = hovered;
    }

    void setPressed(bool pressed) {
        this->pressed = pressed;
    }

    bool isPressed() const {
        return this->pressed;
    }

    void setActive(bool active) {
        this->active = active;
    }

    bool isActive() const {
        return this->active;
    }

    int getHeight() const {
        return h;
    }

    int getWidth() const {
        return w;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

protected:
    WorkSceneController* controller;
    float m_radius;
    bool hovered = false, pressed = false, active = false;
    unsigned int x = 0, y = 0, w = 0, h = 0;
};

#endif // UI_COMPONENT_H
