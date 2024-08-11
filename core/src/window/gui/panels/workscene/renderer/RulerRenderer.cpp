#include "window/gui/panels/workscene/renderer/RulerRenderer.h"

RulerRenderer::RulerRenderer(std::shared_ptr<GLHelper> shader) : shader(shader), rulerVertices(), VAO(0), VBO(0) {}

RulerRenderer::~RulerRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void RulerRenderer::setupRuler() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    if (!rulerVertices.empty())
        glBufferData(GL_ARRAY_BUFFER, rulerVertices.size() * sizeof(float), &rulerVertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void RulerRenderer::renderRuler(SceneCamera* camera) {
    // Prepare ruler vertices
    glm::vec2 cameraPos = camera->GetPosition();
    float initialX = cameraPos.x;
    float finalX = cameraPos.x + camera->GetViewWidth();

    float initialY = cameraPos.y;
    float finalY = cameraPos.y + camera->GetViewHeight();

    float zoomLevel = camera->GetZoom();

    // Ruler steps
    int step;
    if (zoomLevel < 0.2f) step = 100;
    else if (zoomLevel < 0.6f) step = 50;
    else if (zoomLevel < 1.0f) step = 20;
    else step = 10;
    int majorStep = step * 5;

    rulerVertices.clear();
    rulerVertices.reserve(200);
    rulerVertices.insert(rulerVertices.end(), {initialX, 0, finalX, 0, 0, initialY, 0, finalY});

    int initialRulerX = initialX - static_cast<int>(cameraPos.x) % step;
    int initialRulerY = initialY - static_cast<int>(cameraPos.y) % step;

    float majorStepOffset = -10.0f / zoomLevel;
    float majorLineLength = 20.0f / zoomLevel;
    float minorLineLength = 10.0f / zoomLevel;

    // Precompute transformation matrices
    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 horizontalTranslate = glm::translate(identity, glm::vec3(0.0f, finalY - 10.0f / zoomLevel, 0.0f));
    glm::mat4 verticalTranslate = glm::translate(identity, glm::vec3(initialX, 0.0f, 0.0f));

    // Horizontal ruler lines
    for (int t = initialRulerX; t < finalX + step; t += step) {
        bool isMajor = (t % majorStep == 0);
        rulerVertices.push_back(t);
        rulerVertices.push_back(isMajor ? majorStepOffset : 0.0f);
        rulerVertices.push_back(t);
        rulerVertices.push_back(isMajor ? majorLineLength : minorLineLength);
    }

    size_t offset = rulerVertices.size() / 2;

    // Vertical ruler lines
    for (int t = initialRulerY; t < finalY + step; t += step) {
        bool isMajor = (t % majorStep == 0);
        rulerVertices.push_back(isMajor ? majorStepOffset : 0.0f);
        rulerVertices.push_back(t);
        rulerVertices.push_back(isMajor ? majorLineLength : minorLineLength);
        rulerVertices.push_back(t);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, rulerVertices.size() * sizeof(float), &rulerVertices[0], GL_DYNAMIC_DRAW);

    glBindVertexArray(VAO);

    GLHelper::useShader(shader->getProgram());
    GLHelper::setProjectionMatrix(camera->GetProjection());
    GLHelper::setViewMatrix(camera->GetViewMatrix());
    GLHelper::setColor3f(Colors::Gray);
    GLHelper::disableTexture();

    // Cross lines
    GLHelper::setModelMatrix(identity);
    glDrawArrays(GL_LINES, 0, 4);

    // Horizontal ruler lines
    GLHelper::setModelMatrix(horizontalTranslate);
    glDrawArrays(GL_LINES, 4, offset - 4);

    // Vertical ruler lines
    GLHelper::setModelMatrix(verticalTranslate);
    glDrawArrays(GL_LINES, offset, rulerVertices.size() - offset);

    // Ruler text
    float scale = 0.35f / zoomLevel;
    float textOffsetY = finalY - 35.0f / zoomLevel;
    float textOffsetX = initialX + 25.0f / zoomLevel;

    for (int t = initialRulerX; t < finalX + step; t += step) {
        if (t % majorStep == 0) {
            float textWidth = shader->getTextWidth(std::to_string(t), scale);
            shader->renderText(std::to_string(t), t - textWidth / 2, textOffsetY, scale, Colors::White);
        }
    }

    for (int t = initialRulerY; t < finalY + step; t += step) {
        if (t % majorStep == 0) {
            float textHeight = shader->getTextHeight(std::to_string(t), scale);
            shader->renderText(std::to_string(t), textOffsetX, t - textHeight / 2, scale, Colors::White);
        }
    }

    glBindVertexArray(0);
    GLHelper::unuseShader();
}
