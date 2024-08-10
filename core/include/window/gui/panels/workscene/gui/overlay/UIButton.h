#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "window/gui/panels/workscene/gui/overlay/UIComponent.h"
#include "window/gui/panels/workscene/gui/overlay/PanelRenderUtils.h"
#include "window/gui/panels/workscene/controller/WorkSceneController.h"
#include "graphics/utils/stb_image.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

class UIButton : public UIComponent {
public:
    UIButton(WorkSceneController* controller, float radius, const unsigned char* icon, unsigned int icon_len, std::function<void()> onClickListener, WorkSceneMode associatedMode);
    ~UIButton();

    void render(int shaderProgram, float x, float y, float width, float height) override;
    void onClick() override;

    WorkSceneMode getMode() const { return mode; }

private:
    const unsigned char* m_icon;
    unsigned int m_icon_len;
    unsigned int m_iconTexture = 0;
    unsigned int m_VAO = 0, m_iconVAO = 0;
    unsigned int m_VBO = 0, m_iconVBO = 0;

    int m_iconWidth = 0;
    int m_iconHeight = 0;

    std::function<void()> onClickListener;

    WorkSceneMode mode;

    void loadIcon();
    void setupButtonBuffers();
    void setupIconBuffers();
    void renderButton();
    void renderIcon();
};

#endif // UI_BUTTON_H
