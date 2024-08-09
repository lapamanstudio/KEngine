#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "window/gui/panels/workscene/gui/overlay/UIComponent.h"
#include "window/gui/panels/workscene/gui/overlay/PanelRenderUtils.h"
#include "graphics/utils/stb_image.h"

#include <memory>
#include <string>
#include <vector>
#include <functional>

class UIButton : public UIComponent {
public:
    UIButton(WorkSceneController* controller, float radius, const unsigned char* icon, unsigned int icon_len, std::function<void()> onClickListener = nullptr);
    ~UIButton();

    void render(int shaderProgram, float x, float y, float width, float height) override;
    void onClick() override;

private:
    const unsigned char* m_icon;
    unsigned int m_icon_len, m_iconTexture = 0, m_VAO = 0, m_VBO = 0;
    int m_iconWidth = 0, m_iconHeight = 0;

    std::function<void()> onClickListener;

    void loadIcon();
    void setupButtonBuffers();
    void renderButton(float x, float y, float width, float height);
    void renderIcon(int shaderProgram, float x, float y, float width, float height);
};

#endif // UI_BUTTON_H
