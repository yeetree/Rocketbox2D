#include "Engine/Platform/IPlatform.h"
#include "Engine/Platform/IGraphicsBridge.h"
#include "Platform/SDL3/SDL3Platform.h"

namespace Engine
{
    Scope<IPlatform> IPlatform::Create(GraphicsAPI api) {
        #if defined(ENGINE_PLATFORM_WINDOWS) || defined(ENGINE_PLATFORM_LINUX)
            return CreateScope<SDL3Platform>(IGraphicsBridge::Create(api));
        #endif
    }
} // namespace Engine
