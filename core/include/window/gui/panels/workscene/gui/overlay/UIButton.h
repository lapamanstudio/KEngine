#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "window/gui/panels/workscene/gui/overlay/UIComponent.h"
#include "window/gui/panels/workscene/gui/overlay/PanelRenderUtils.h"
#include "window/gui/panels/workscene/controller/WorkSceneController.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

class UIButton : public UIComponent {
public:
    UIButton(WorkSceneController* controller, float radius, const char* icon_path, std::function<void()> onClickListener, WorkSceneMode associatedMode);
    ~UIButton();

    void render(int shaderProgram, float x, float y, float width, float height) override;
    void onClick() override;

    WorkSceneMode getMode() const { return mode; }

private:
    const char* m_icon_path;
    unsigned int m_iconTexture = 0;
    unsigned int m_VAO = 0, m_VBO = 0;

    std::function<void()> onClickListener;

    WorkSceneMode mode;

    void setupButtonBuffers();
    void renderButton();
};

#endif // UI_BUTTON_H
