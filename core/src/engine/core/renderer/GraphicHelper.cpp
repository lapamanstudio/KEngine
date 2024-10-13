#include "engine/core/renderer/GraphicHelper.h"
#include "engine/core/renderer/GraphicHelperFactory.h"

GraphicHelper* GraphicHelper::instance = nullptr;

void GraphicHelper::Init(RenderAPI api) {
    if (!instance) {
        instance = GraphicHelperFactory::CreateGraphicHelper(api);
    }
}
