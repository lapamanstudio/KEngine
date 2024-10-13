#ifndef GRAPHIC_HELPER_FACTORY_H
#define GRAPHIC_HELPER_FACTORY_H

#include "engine/core/renderer/GraphicHelper.h"

#include "engine/platform/opengl/GLGraphicHelper.h"
#include "engine/platform/directx/DXGraphicHelper.h"
#include "engine/graphics/WindowFactory.h"

class GraphicHelperFactory {
public:
    static GraphicHelper* CreateGraphicHelper(RenderAPI api) {
        switch (api) {
            case RenderAPI::OpenGL:
                return new GLGraphicHelper();
            case RenderAPI::DirectX:
                // return new DXGraphicHelper();
            default:
                return nullptr;
        }
    }
};

#endif // GRAPHIC_HELPER_FACTORY_H
