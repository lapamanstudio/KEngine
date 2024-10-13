#ifndef RENDERER_H
#define RENDERER_H

#include "Transform.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void Render(Transform* transform);
};

#endif // RENDERER_H
