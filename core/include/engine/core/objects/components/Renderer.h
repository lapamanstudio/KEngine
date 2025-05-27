#pragma once

#include "engine/core/objects/components/Transform.h"

#include <GL/glew.h> // TODO Change when implementing DirectX

class Renderer {
public:
    Renderer();
    ~Renderer();
    void Render(Transform* transform);

private:
    void SetTexture(GLuint textureID) { this->textureID = textureID; }
    
    GLuint textureID;
};
