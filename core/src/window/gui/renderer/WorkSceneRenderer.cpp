#include "window/gui/renderer/WorkSceneRenderer.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/drivers/ShaderHelper.h"

#include <cstdio>

WorkSceneRenderer::WorkSceneRenderer(bool* debug, int posX, int posY, int width, int height) 
    : debug(debug), posX(posX), posY(posY), width(width), height(height), texture_id(0), shaderProgram(0), VAO(0), VBO(0), EBO(0), FBO(0) {

    shader = std::make_shared<GLHelper>(vertexShaderSource, fragmentShaderSource, textShaderSource, textFragmentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    updateSize(posX, posY, width, height);
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

void WorkSceneRenderer::render(SceneCamera* camera, SceneManager* sceneManager) {
    GLHelper::useShader(shader->getTextProgram());
    GLHelper::setProjectionMatrix(projection);
    GLHelper::setViewMatrix(camera->GetViewMatrix());
    GLHelper::unuseShader();

    GLHelper::useShader(shader->getProgram());

    GLHelper::setModelMatrix(glm::mat4(1.0f));
    GLHelper::setProjectionMatrix(projection);
    GLHelper::setViewMatrix(camera->GetViewMatrix());

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(posX, posY, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    batchRender(camera, sceneManager);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLHelper::unuseShader();
}

void WorkSceneRenderer::batchRender(SceneCamera* camera, SceneManager* sceneManager) {
    clearVertexData();

    // Get vertex data from all objects in the scene
    if (sceneManager == nullptr || sceneManager->GetObjects().empty())
        return;
    
    glBindVertexArray(VAO);
    
    for (const auto& object : sceneManager->GetObjects()) {
        // Render object name
        shader->renderText("Camera", 0, object->GetSize().g + 5, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

        // Render object
        object->Render(shader->getProgram());
    }

    if (debug && *debug) {
        float offsetX = width / 2.0f;
        float offsetY = height / 2.0f;
        float topLeftX = -offsetX;
        float topLeftY = height - offsetY;

        GLHelper::useShader(shader->getTextProgram());
        GLHelper::setProjectionMatrix(projection);
        GLHelper::setViewMatrix(glm::mat4(1.0f));

        shader->renderText("Position X: " + std::to_string(camera->GetPosition().x), topLeftX, topLeftY - 20, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
        shader->renderText("Position Y: " + std::to_string(camera->GetPosition().y), topLeftX, topLeftY - 40, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
        shader->renderText("Zoom: " + std::to_string(camera->GetZoom()), topLeftX, topLeftY - 60, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));

        GLHelper::unuseShader();
    }

    glBindVertexArray(0);
}

void WorkSceneRenderer::clearVertexData() {
    vertexData.clear();
}

GLuint WorkSceneRenderer::getTexture() const {
    return texture_id;
}

void WorkSceneRenderer::updateSize(int newX, int newY, int newWidth, int newHeight) {
    posX = newX;
    posY = newY;
    width = newWidth;
    height = newHeight;

    // TODO: Remove offset and move the camera when initializing (to look better)
    float offsetX = newWidth / 2.0f;
    float offsetY = newHeight / 2.0f;

    projection = glm::ortho(
        -offsetX,
        static_cast<float>(newWidth) - offsetX,
        static_cast<float>(newHeight) - offsetY,
        -offsetY,
        -1.0f,
        1.0f
    );

    setupFramebuffer();
}
