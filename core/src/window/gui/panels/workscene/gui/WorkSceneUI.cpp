#include "window/gui/panels/workscene/gui/WorkSceneUI.h"

void WorkSceneUI::render(GLuint shaderProgram, int width, int height) {
    GLHelper::useShader(shaderProgram);

    GLHelper::setModelMatrix(glm::mat4(1.0f));
    GLHelper::setViewMatrix(glm::mat4(1.0f));
    GLHelper::disableTexture();

    // Calculate the position of the buttons to be centered
    int spacing = 10;
    int totalHeight = 0;
    for (unsigned int i = 0; i < buttons.size(); i++)
        totalHeight += buttons[i].get()->getHeight() + spacing;

    int y = (height - totalHeight) / 2;

    for (unsigned int i = 0; i < buttons.size(); i++) {
        int x = width - 50;
        buttons[i].get()->render(shaderProgram, x, y, 40, 40);
        y += buttons[i].get()->getHeight() + spacing;
    }

    GLHelper::unuseShader();
}

void WorkSceneUI::addButton(std::shared_ptr<UIButton> button) {
    buttons.push_back(button);
}

std::vector<std::shared_ptr<UIButton>> WorkSceneUI::getButtons() {
    return buttons;
}
