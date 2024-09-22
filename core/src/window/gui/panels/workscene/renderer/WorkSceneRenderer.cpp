#include "window/gui/panels/workscene/renderer/WorkSceneRenderer.h"
#include "window/gui/panels/workscene/gui/overlay/PanelRenderUtils.h"
#include "graphics/drivers/GLHelper.h"
#include "graphics/scene/objects/EmptyObject.h"
#include "graphics/utils/TextureManager.h"
#include "graphics/utils/Colors.h"

#include <cstdio>

WorkSceneRenderer::WorkSceneRenderer(std::shared_ptr<GLHelper> shader, int posX, int posY, int width, int height) :
    rulerRenderer(std::make_shared<RulerRenderer>(shader)),
    shader(shader),
    vertexData(),
    selectionBox(glm::vec4(0.0f)),
    posX(posX),
    posY(posY),
    width(width),
    height(height),
    texture_id(0),
    VAO(0),
    VBO(0) {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    translationUpArrowTexture = TextureManager::LoadTexture("icons/translation_up_arrow.png");
    translationRigthArrowTexture = TextureManager::LoadTexture("icons/translation_right_arrow.png");
    translationCenterCircleTexture = TextureManager::LoadTexture("icons/translation_center_circle.png");
}

WorkSceneRenderer::~WorkSceneRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture_id);
}

void WorkSceneRenderer::render(SceneCamera* camera, SceneManager* sceneManager) {
    GLHelper::useShader(shader->getTextProgram());
    GLHelper::setProjectionMatrix(camera->GetProjection());
    GLHelper::setViewMatrix(camera->GetViewMatrix());
    GLHelper::disableTexture();

    glClearColor(0.04f, 0.04f, 0.04f, 1.0f);
    shader->prepareRender(width, height, 2);

    // As the width and height are doubled, the line width should be doubled as well
    glLineWidth(3.0f);

    // Ruler
    rulerRenderer->renderRuler(camera);

    // Scene
    batchRender(camera, sceneManager);

    GLHelper::unuseShader();
}

void WorkSceneRenderer::postRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WorkSceneRenderer::batchRender(SceneCamera* camera, SceneManager* sceneManager) {
    GLHelper::useShader(shader->getProgram());

    GLHelper::setModelMatrix(glm::mat4(1.0f));
    GLHelper::setProjectionMatrix(camera->GetProjection());
    GLHelper::setViewMatrix(camera->GetViewMatrix());
    GLHelper::disableTexture();

    vertexData.clear();

    // Get vertex data from all objects in the scene
    if (sceneManager != nullptr && !sceneManager->GetObjects().empty()) {
        glBindVertexArray(EmptyObject::GetVAO());

        // Render objects
        for (const auto& object : sceneManager->GetObjects())
            object->Render(shader->getProgram());

        // Render selection boxes of active objects
        for (const auto& activeObject : sceneManager->GetActiveObjects())
            activeObject->RenderSelectedBox(shader->getProgram());

        glBindVertexArray(0);

        // The model was changed in the selection box of the active object, so we need to reset it
        GLHelper::setModelMatrix(glm::mat4(1.0f));
    }

    // Render cursor selection box
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

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), &boxVertices[0], GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        GLHelper::disableTexture();
        GLHelper::setColor3f(Colors::Gray);

        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);
    }

    if (sceneManager->GetMode() == TRANSLATION_MODE) {
        auto middleOpt = sceneManager->GetMiddlePositionOfActiveObjects();
        if (!middleOpt.has_value()) return;

        glm::vec2 middle = middleOpt.value();

        // TODO: The positions are wrong when changed the icons (cause the icons were paint designed)
        GLHelper::setColor3f(sceneManager->IsMouseInRightArrow() ? Colors::Green : Colors::White);
        TextureManager::renderTexture(translationRigthArrowTexture, middle.x + 20, middle.y - 64, 128, 128);
        GLHelper::setColor3f(sceneManager->IsMouseInUpperArrow() ? Colors::Green : Colors::White);
        TextureManager::renderTexture(translationUpArrowTexture, middle.x - 64, middle.y + 10, 128, 128);
        GLHelper::setColor3f(sceneManager->IsMouseInCenterCircle() ? Colors::Green : Colors::White);
        TextureManager::renderTexture(translationCenterCircleTexture, middle.x - 64, middle.y - 64, 128, 128);
    }
}

void WorkSceneRenderer::updateSize(SceneCamera* camera, int newWidth, int newHeight) {
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

    shader->setupFramebuffer(texture_id, newWidth, newHeight, 2);
    rulerRenderer->setupRuler();
}
