#include "window/gui/renderer/WorkSceneRenderer.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/drivers/ShaderHelper.h"
#include "graphics/utils/Colors.h"

#include <cstdio>

WorkSceneRenderer::WorkSceneRenderer(bool* debug, int posX, int posY, int width, int height) :
    vertexData(),
    rulerVertices(),
    selectionBox(glm::vec4(0.0f)),
    debug(debug),
    posX(posX),
    posY(posY),
    width(width),
    height(height),
    texture_id(0),
    shaderProgram(0),
    VAO(0),
    VBO(0),
    EBO(0),
    FBO(0) {

    shader = std::make_shared<GLHelper>(vertexShaderSource, fragmentShaderSource, textShaderSource, textFragmentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

WorkSceneRenderer::~WorkSceneRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &texture_id);
    glDeleteProgram(shaderProgram);
}

void WorkSceneRenderer::setupFramebuffer() {
    if (FBO) {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WorkSceneRenderer::setupRuler(SceneCamera* camera) {
    if (VAO == 0)
        glGenVertexArrays(1, &VAO);
    if (VBO == 0)
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

void WorkSceneRenderer::renderRuler(SceneCamera* camera) {
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

    GLHelper::useShader(shader->getProgram());
    glBindVertexArray(VAO);

    GLHelper::setProjectionMatrix(camera->GetProjection());
    GLHelper::setViewMatrix(camera->GetViewMatrix());
    GLHelper::setColor3f(Colors::Gray);

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

void WorkSceneRenderer::renderDebugIfNeeded(SceneCamera* camera) {
    if (this->debug && *this->debug) {
        float topLeftX = 0;
        float topLeftY = this->height;

        GLHelper::useShader(shader->getTextProgram());
        GLHelper::setProjectionMatrix(camera->GetProjection());
        GLHelper::setViewMatrix(glm::mat4(1.0f));

        shader->renderText("Camera position X: " + std::to_string(camera->GetPosition().x), topLeftX, topLeftY - 20, 0.3f, Colors::White);
        shader->renderText("Camera position Y: " + std::to_string(camera->GetPosition().y), topLeftX, topLeftY - 40, 0.3f, Colors::White);
        shader->renderText("Camera end position X: " + std::to_string(camera->GetPosition().x + camera->GetViewWidth()), topLeftX, topLeftY - 60, 0.3f, Colors::White);
        shader->renderText("Camera top position Y: " + std::to_string(camera->GetPosition().y + camera->GetViewHeight()), topLeftX, topLeftY - 80, 0.3f, Colors::White);
        shader->renderText("Camera zoom: " + std::to_string(camera->GetZoom()), topLeftX, topLeftY - 100, 0.3f, Colors::White);
        shader->renderText("Camera width: " + std::to_string(camera->GetViewWidth()), topLeftX, topLeftY - 120, 0.3f, Colors::White);
        shader->renderText("Camera height: " + std::to_string(camera->GetViewHeight()), topLeftX, topLeftY - 140, 0.3f, Colors::White);

        GLHelper::unuseShader();
    }
}

void WorkSceneRenderer::render(SceneCamera* camera, SceneManager* sceneManager) {
    GLHelper::useShader(shader->getTextProgram());
    GLHelper::setProjectionMatrix(camera->GetProjection());
    GLHelper::setViewMatrix(camera->GetViewMatrix());
    GLHelper::unuseShader();

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(posX, posY, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    // Ruler
    renderRuler(camera);

    // Scene
    batchRender(camera, sceneManager);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLHelper::unuseShader();
}

void WorkSceneRenderer::batchRender(SceneCamera* camera, SceneManager* sceneManager) {
    GLHelper::useShader(shader->getProgram());

    GLHelper::setModelMatrix(glm::mat4(1.0f));
    GLHelper::setProjectionMatrix(camera->GetProjection());
    GLHelper::setViewMatrix(camera->GetViewMatrix());

    clearVertexData();

    // Get vertex data from all objects in the scene
    if (sceneManager == nullptr || sceneManager->GetObjects().empty())
        return;
    
    glBindVertexArray(VAO);
    
    for (const auto& object : sceneManager->GetObjects()) {
        // Render object
        object->Render(shader->getProgram());

        // If is active object, render the selection box
        if (sceneManager->GetActiveObject() == object)
            object->RenderSelectionBox(shader->getProgram());
    }

    // Render selection box
    if (selectionBox.x != 0.0f && selectionBox.y != 0.0f && selectionBox.z != 0.0f && selectionBox.w != 0.0f) {
        float x1 = selectionBox.x;
        float y1 = selectionBox.y;
        float x2 = selectionBox.z;
        float y2 = selectionBox.w;

        std::vector<float> boxVertices = {
            x1, y1,
            x2, y1,
            x2, y2,
            x1, y2
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), &boxVertices[0], GL_DYNAMIC_DRAW);

        GLHelper::useShader(shader->getProgram());

        glBindVertexArray(VAO);

        GLHelper::setModelMatrix(glm::mat4(1.0f));
        GLHelper::setProjectionMatrix(camera->GetProjection());
        GLHelper::setViewMatrix(camera->GetViewMatrix());
        GLHelper::setColor3f(Colors::Gray);

        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glBindVertexArray(0);

        GLHelper::unuseShader();
    }

    renderDebugIfNeeded(camera);

    glBindVertexArray(0);
}

void WorkSceneRenderer::clearVertexData() {
    vertexData.clear();
}

GLuint WorkSceneRenderer::getTexture() const {
    return texture_id;
}

void WorkSceneRenderer::updateSize(SceneCamera* camera, int newX, int newY, int newWidth, int newHeight) {
    posX = newX;
    posY = newY;
    width = newWidth;
    height = newHeight;

    camera->SetProjection(glm::ortho(
        0.0f,
        static_cast<float>(newWidth),
        static_cast<float>(newHeight),
        0.0f,
        -1.0f,
        1.0f
    ));

    setupFramebuffer();
    setupRuler(camera);
}
