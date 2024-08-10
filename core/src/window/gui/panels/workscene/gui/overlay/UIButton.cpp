#include "window/gui/panels/workscene/gui/overlay/UIButton.h"
#include "window/gui/panels/workscene/controller/WorkSceneController.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/utils/Colors.h"

UIButton::UIButton(WorkSceneController* controller, float radius, const unsigned char* icon, unsigned int icon_len, std::function<void()> onClickListener, WorkSceneMode associatedMode)
    : UIComponent(controller, radius), m_icon(icon), m_icon_len(icon_len), onClickListener(onClickListener), mode(associatedMode) {
    loadIcon();
    setupButtonBuffers();
    setupIconBuffers();
}

UIButton::~UIButton() {
    if (m_iconTexture) glDeleteTextures(1, &m_iconTexture);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_iconVAO) glDeleteVertexArrays(1, &m_iconVAO);
    if (m_iconVBO) glDeleteBuffers(1, &m_iconVBO);
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

void UIButton::loadIcon() {
    unsigned char* data = stbi_load_from_memory(m_icon, m_icon_len, &m_iconWidth, &m_iconHeight, nullptr, 4);
    if (data) {
        glGenTextures(1, &m_iconTexture);
        glBindTexture(GL_TEXTURE_2D, m_iconTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iconWidth, m_iconHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    } else {
        std::cerr << "Error loading icon texture\n";
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

void UIButton::setupIconBuffers() {
    glGenVertexArrays(1, &m_iconVAO);
    glGenBuffers(1, &m_iconVBO);

    glBindVertexArray(m_iconVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_iconVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void UIButton::renderButton() {
    std::vector<float> vertices = generateRoundedRectVertices(x, y, w, h, m_radius);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    GLHelper::disableTexture();
    GLHelper::setColor3f(this->active ? Colors::DarkYellow : this->hovered ? Colors::Gray : Colors::DarkGray);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 3);

    GLHelper::setColor3f(Colors::Gray);
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 3);

    glBindVertexArray(0);
}

void UIButton::renderIcon() {
    if (m_iconTexture == 0) return;

    glBindVertexArray(m_iconVAO);

    float iconVertices[] = {
        static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f, 1.0f,
        static_cast<float>(x + w), static_cast<float>(y), 0.0f, 1.0f, 1.0f,
        static_cast<float>(x + w), static_cast<float>(y + h), 0.0f, 1.0f, 0.0f,
        static_cast<float>(x), static_cast<float>(y + h), 0.0f, 0.0f, 0.0f
    };

    unsigned int iconIndices[] = { 0, 1, 2, 2, 3, 0 };

    glBindBuffer(GL_ARRAY_BUFFER, m_iconVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(iconVertices), iconVertices, GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_iconTexture);

    GLHelper::setTexture(0);
    GLHelper::setColor3f(Colors::White);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, iconIndices);

    glBindVertexArray(0);
}
