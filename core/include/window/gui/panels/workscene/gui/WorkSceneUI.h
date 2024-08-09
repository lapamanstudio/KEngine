#ifndef WORKSCENEUI_H
#define WORKSCENEUI_H

#include "window/gui/panels/workscene/gui/overlay/UIButton.h"
#include "window/gui/panels/workscene/controller/WorkSceneController.h"

class WorkSceneUI {
public:
    WorkSceneUI() = default;
    ~WorkSceneUI() = default;

    void render(GLuint shaderProgram, int width, int height);
    void addButton(std::shared_ptr<UIButton> button);
    std::vector<std::shared_ptr<UIButton>> getButtons();

private:
    std::vector<std::shared_ptr<UIButton>> buttons;
};

#endif // WORKSCENEUI_H
