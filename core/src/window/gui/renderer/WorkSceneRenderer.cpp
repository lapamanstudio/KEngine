#include "window/gui/renderer/WorkSceneRenderer.h"
#include "graphics/drivers/GLHelper.h"

#include <cstdio>

// Vertex Shader source code
const GLchar* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
)";

// Fragment Shader source code
const GLchar* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 vertexColor;

void main() {
    FragColor = vertexColor;
}
)";

WorkSceneRenderer::WorkSceneRenderer(int posX, int posY, int width, int height) 
    : posX(posX), posY(posY), width(width), height(height), texture_id(0), shaderProgram(0), VAO(0), VBO(0), EBO(0), FBO(0) {

    shader = std::make_shared<GLHelper>(vertexShaderSource, fragmentShaderSource);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WorkSceneRenderer::render(SceneCamera* camera, SceneManager* sceneManager) {
    GLHelper::useShader(shader->getProgram());

    GLHelper::setModelMatrix(glm::mat4(1.0f));
    GLHelper::setProjectionMatrix(projection);
    GLHelper::setViewMatrix(camera->GetViewMatrix());

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(posX, posY, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    batchRender(sceneManager);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLHelper::unuseShader();
}

void WorkSceneRenderer::batchRender(SceneManager* sceneManager) {
    clearVertexData();

    // Get vertex data from all objects in the scene
    if (sceneManager == nullptr || sceneManager->GetObjects().empty())
        return;
    
    glBindVertexArray(VAO);
    
    for (const auto& object : sceneManager->GetObjects()) {
        object->Render(shader->getProgram());
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

    projection = glm::ortho(-static_cast<float>(newWidth) / 2.0f, static_cast<float>(newWidth) / 2.0f, 
                            -static_cast<float>(newHeight) / 2.0f, static_cast<float>(newHeight) / 2.0f, 
                            -1.0f, 1.0f);

    setupFramebuffer();
}
