#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Renderer/Vulkan/VulkanGraphicsDevice.h"

namespace Engine
{
    // Static function to create GraphicsDevice with selected graphics api
    Scope<IGraphicsDevice> IGraphicsDevice::Create(GraphicsAPI api, SDL_Window* window) {
        switch (api) {
            case GraphicsAPI::Vulkan: return CreateScope<VulkanGraphicsDevice>(window);
        }
        return nullptr;
    }
} // namespace Engine
