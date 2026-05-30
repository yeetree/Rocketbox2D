#include "Engine/RHI/IGraphicsDevice.h"
#include "Engine/Platform/IGraphicsBridge.h"

#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "Platform/SDL3/Vulkan/SDL3VulkanGraphicsBridge.h"

namespace Engine::RHI
{
    // Static function to create GraphicsDevice with selected graphics api
    Scope<IGraphicsDevice> IGraphicsDevice::Create(GraphicsAPI api) {
        switch (api) {
            case GraphicsAPI::Vulkan: {

                Scope<Vulkan::IVulkanGraphicsBridge> bridge;
                
                #if defined(ENGINE_PLATFORM_WINDOWS) | defined(ENGINE_PLATFORM_LINUX)
                bridge = CreateScope<Vulkan::SDL3::SDL3VulkanGraphicsBridge>();
                #endif

                return CreateScope<Vulkan::VulkanGraphicsDevice>(std::move(bridge));
                break;
            }
        }
        return nullptr;
    }
} // namespace Engine::RHI