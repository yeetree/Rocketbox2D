#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Renderer/Vulkan/RHI/VulkanGraphicsDevice.h"

namespace Engine
{
    // Static function to create GraphicsDevice with selected graphics api
    Scope<IGraphicsDevice> IGraphicsDevice::Create(GraphicsAPI api, IGraphicsBridge* graphicsBridge, IWindow* window) {
        switch (api) {
            case GraphicsAPI::Vulkan: return CreateScope<VulkanGraphicsDevice>(graphicsBridge, window);
        }
        return nullptr;
    }
} // namespace Engine
