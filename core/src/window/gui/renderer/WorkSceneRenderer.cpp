#include "window/gui/renderer/WorkSceneRenderer.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/drivers/ShaderHelper.h"
#include "graphics/utils/Colors.h"

#include <cstdio>

WorkSceneRenderer::WorkSceneRenderer(bool* debug, int posX, int posY, int width, int height) 
    : debug(debug), posX(posX), posY(posY), width(width), height(height), texture_id(0), shaderProgram(0), VAO(0), VBO(0), EBO(0), FBO(0) {

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
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, rulerVertices.size() * sizeof(float), &rulerVertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void WorkSceneRenderer::renderRuler(SceneCamera* camera) {
    // Prepare ruler vertices
    float initialX = camera->GetPosition().x;
    float finalX = camera->GetPosition().x + camera->GetViewWidth();

    float initialY = camera->GetPosition().y;
    float finalY = camera->GetPosition().y + camera->GetViewHeight();

    rulerVertices.clear();
    rulerVertices = {
        initialX, 0,
        finalX, 0,
        0, initialY,
        0, finalY
    };

    int initialRulerX = initialX - ((int) (camera->GetPosition().x) % 10);
    for (int t = initialRulerX; t < finalX + 10; t += 10) {
        rulerVertices.push_back(t);
        rulerVertices.push_back(t % 50 == 0 ? -10 : 0);

        rulerVertices.push_back(t);
        rulerVertices.push_back(t % 50 == 0 ? 20 : 10);
    }

    int offset = rulerVertices.size() / 2;

    int initialRulerY = initialY - ((int) (camera->GetPosition().y) % 10);
    for (int t = initialRulerY; t < finalY + 10; t += 10) {
        rulerVertices.push_back(t % 50 == 0 ? -10 : 0);
        rulerVertices.push_back(t);

        rulerVertices.push_back(t % 50 == 0 ? 20 : 10);
        rulerVertices.push_back(t);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, rulerVertices.size() * sizeof(float), &rulerVertices[0], GL_DYNAMIC_DRAW);

    GLHelper::useShader(shader->getProgram());

    glBindVertexArray(VAO);

    GLHelper::setModelMatrix(glm::mat4(1.0f));
    GLHelper::setProjectionMatrix(camera->GetProjection());
    GLHelper::setViewMatrix(camera->GetViewMatrix());
    GLHelper::setColor3f(Colors::Gray);

    // Cross lines
    glDrawArrays(GL_LINES, 0, 4);

    GLHelper::setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, finalY - 10, 0.0f)));

    // Horizontal ruler lines
    glDrawArrays(GL_LINES, 4, offset - 8);

    GLHelper::setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(initialX, 0.0f, 0.0f)));

    // Vertical ruler lines
    glDrawArrays(GL_LINES, offset, rulerVertices.size() - offset - 8);

    // Ruler text
    float scale = 0.3f;

    for (int t = initialRulerX; t < finalX + 10; t += 10) {
        if (t % 50 == 0) {
            // Center the text
            float textWidth = shader->getTextWidth(std::to_string(t), scale);

            shader->renderText(std::to_string(t), t - textWidth / 2, finalY - 35, scale, Colors::White);
        }
    }

    for (int t = initialRulerY; t < finalY + 10; t += 10) {
        // Center the text
        float textHeight = shader->getTextHeight(std::to_string(t), scale);
        
        if (t % 50 == 0) {
            shader->renderText(std::to_string(t), initialX + 25, t - textHeight / 2, scale, Colors::White);
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
        // Render object name
        shader->renderText("Camera", 0, object->GetSize().g + 5, 0.5f, Colors::White);

        // Render object
        object->Render(shader->getProgram());
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
