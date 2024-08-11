#include "window/gui/panels/workscene/gui/overlay/UIButton.h"
#include "window/gui/panels/workscene/controller/WorkSceneController.h"
#include "graphics/utils/TextureManager.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/Colors.h"

UIButton::UIButton(WorkSceneController* controller, float radius, const unsigned char* icon, unsigned int icon_len, std::function<void()> onClickListener, WorkSceneMode associatedMode)
    : UIComponent(controller, radius), m_icon(icon), m_icon_len(icon_len), onClickListener(onClickListener), mode(associatedMode) {
    m_iconTexture = TextureManager::LoadTexture(m_icon, m_icon_len);
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
    renderIcon();
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

void UIButton::renderIcon() {
    if (m_iconTexture == 0) return;

    TextureManager::BindVAO();

    float iconVertices[] = {
        static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f, 1.0f,
        static_cast<float>(x + w), static_cast<float>(y), 0.0f, 1.0f, 1.0f,
        static_cast<float>(x + w), static_cast<float>(y + h), 0.0f, 1.0f, 0.0f,
        static_cast<float>(x), static_cast<float>(y + h), 0.0f, 0.0f, 0.0f
    };

    unsigned int iconIndices[] = { 0, 1, 2, 2, 3, 0 };

    TextureManager::BindVBO();
    glBufferData(GL_ARRAY_BUFFER, sizeof(iconVertices), iconVertices, GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_iconTexture);

    GLHelper::setTexture(0);
    GLHelper::setColor3f(Colors::White);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, iconIndices);

    glBindVertexArray(0);
}
