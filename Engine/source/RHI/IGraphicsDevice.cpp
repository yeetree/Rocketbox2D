#include "Engine/RHI/IGraphicsDevice.h"
#include "Engine/Platform/IGraphicsBridge.h"

#include "RHI/Vulkan/VulkanGraphicsDevice.h"

namespace Engine
{
    // Static function to create GraphicsDevice with selected graphics api
    Scope<IGraphicsDevice> IGraphicsDevice::Create(GraphicsAPI api, Ref<IGraphicsBridge> graphicsBridge, Ref<IWindow> window) {
        switch (api) {
            case GraphicsAPI::Vulkan: return CreateScope<VulkanGraphicsDevice>(graphicsBridge, window);
        }
        return nullptr;
    }
} // namespace Engine