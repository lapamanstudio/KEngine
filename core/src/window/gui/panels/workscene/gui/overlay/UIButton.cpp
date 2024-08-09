#include "window/gui/panels/workscene/gui/overlay/UIButton.h"
#include "window/gui/panels/workscene/controller/WorkSceneController.h"

UIButton::UIButton(WorkSceneController* controller, float radius, const unsigned char* icon, unsigned int icon_len, std::function<void()> onClickListener)
    : UIComponent(controller, radius), m_icon(icon), m_icon_len(icon_len), onClickListener(onClickListener) {
    loadIcon();
    setupButtonBuffers();
}

UIButton::~UIButton() {
    glDeleteTextures(1, &m_iconTexture);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void UIButton::render(int shaderProgram, float x, float y, float width, float height) {
    this->x = x;
    this->y = y;
    this->w = width;
    this->h = height;

    renderButton(x, y, width, height);
    // renderIcon(shaderProgram, x, y, width, height);
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
        printf("Error loading icon texture\n");
    }
}

void UIButton::setupButtonBuffers() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void UIButton::renderButton(float x, float y, float width, float height) {
    std::vector<float> vertices = generateRoundedRectVertices(x, y, width, height, m_radius);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    GLHelper::setColor3f((controller && controller->getMode() == 1) ? Colors::DarkYellow : this->hovered ? Colors::Gray : Colors::DarkGray);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 3);

    GLHelper::setColor3f(Colors::Gray);
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 3);

    glBindVertexArray(0);
}

void UIButton::renderIcon(int shaderProgram, float x, float y, float width, float height) {
    if (m_iconTexture == 0) return;

    float iconVertices[] = {
        x,            y,              0.0f, 0.0f, 1.0f,
        x + width,    y,              0.0f, 1.0f, 1.0f,
        x + width,    y + height,     0.0f, 1.0f, 0.0f,
        x,            y + height,     0.0f, 0.0f, 0.0f
    };

    unsigned int iconIndices[] = { 0, 1, 2, 2, 3, 0 };

    unsigned int iconVAO, iconVBO, iconEBO;
    glGenVertexArrays(1, &iconVAO);
    glGenBuffers(1, &iconVBO);
    glGenBuffers(1, &iconEBO);

    glBindVertexArray(iconVAO);
    glBindBuffer(GL_ARRAY_BUFFER, iconVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(iconVertices), iconVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iconEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iconIndices), iconIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLHelper::setTexture(0);
    GLHelper::setColor3f(Colors::White);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_iconTexture);

    glBindVertexArray(iconVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &iconVAO);
    glDeleteBuffers(1, &iconVBO);
    glDeleteBuffers(1, &iconEBO);
}
