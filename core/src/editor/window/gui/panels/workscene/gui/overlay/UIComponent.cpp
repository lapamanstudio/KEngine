#include "editor/window/gui/panels/workscene/gui/overlay/UIComponent.h"

UIComponent::UIComponent(WorkSceneController* controller, float radius)
    : controller(controller), m_radius(radius), hovered(false), pressed(false), x(0), y(0), w(0), h(0) {}

UIComponent::~UIComponent() {}

bool UIComponent::isMouseOver(glm::vec2 mousePos) {
    return mousePos.x >= this->x && mousePos.x <= this->x + this->w && mousePos.y >= this->y && mousePos.y <= this->y + this->h;
}
