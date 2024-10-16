#ifndef ENGINE_WINDOWFACTORY_H
#define ENGINE_WINDOWFACTORY_H

#include "engine/graphics/Window.h"

#include "engine/platform/RenderAPI.h"
#include "engine/platform/directx/DXWindow.h"
#include "engine/platform/opengl/GLWindow.h"

class WindowFactory {
public:

    static Window* GetInstance() {
        return window ? window : nullptr;
    }

    static Window* CreateRenderWindow(RenderAPI api) {
        switch (api) {
            case RenderAPI::DirectX:
                return new DXWindow();
            case RenderAPI::OpenGL:
                return new GLWindow();
            default:
                return nullptr;
        }
    }

private:
    static Window* window;
};

#endif //ENGINE_WINDOWFACTORY_H
