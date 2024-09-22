#include "window/gui/panels/workscene/gui/overlay/UIButton.h"
#include "window/gui/panels/workscene/controller/WorkSceneController.h"
#include "graphics/utils/TextureManager.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/Colors.h"

UIButton::UIButton(WorkSceneController* controller, float radius, const char* icon_path, std::function<void()> onClickListener, WorkSceneMode associatedMode)
    : UIComponent(controller, radius), m_icon_path(icon_path), onClickListener(onClickListener), mode(associatedMode) {
    m_iconTexture = TextureManager::LoadTexture(m_icon_path);
    setupButtonBuffers();
}

UIButton::~UIButton() {
    if (m_iconTexture) glDeleteTextures(1, &m_iconTexture);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
}

void UIButton::render(int shaderProgram, float x, float y, float width, float height) {
    this->x = x;
    this->y = y;
    this->w = width;
    this->h = height;

    glUseProgram(shaderProgram);

    renderButton();

    GLHelper::setColor3f(Colors::White);
    TextureManager::renderTexture(m_iconTexture, x, y, width, height);
}

void UIButton::onClick() {
    if (onClickListener) {
        onClickListener();
    }
}

void UIButton::setupButtonBuffers() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void UIButton::renderButton() {
    std::vector<float> vertices = generateRoundedRectVertices(x, y, w, h, m_radius);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    GLHelper::disableTexture();
    GLHelper::setColor3f(this->active ? Colors::Orange : this->hovered ? Colors::Gray : Colors::DarkGray);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 3);

    GLHelper::setColor3f(Colors::Gray);
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 3);

    glBindVertexArray(0);
}
