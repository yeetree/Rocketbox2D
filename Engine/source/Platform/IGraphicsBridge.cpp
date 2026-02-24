#include "Engine/Platform/IGraphicsBridge.h"
#include "Platform/SDL3/Vulkan/SDL3VulkanGraphicsBridge.h"

namespace Engine
{
    Scope<IGraphicsBridge> IGraphicsBridge::Create(GraphicsAPI api) {
        switch(api) {
            case GraphicsAPI::Vulkan: {
                #if defined(ENGINE_PLATFORM_WINDOWS) | defined(ENGINE_PLATFORM_LINUX)
                return CreateScope<SDL3VulkanGraphicsBridge>();
                #endif

                break;
            }
        }
        return nullptr; // Shouldn't really happen...
    }
} // namespace Engine
