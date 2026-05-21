#ifndef PLATFORM_SDL3_VULKAN_SDL3VULKANGRAPHICSINSTANCE
#define PLATFORM_SDL3_VULKAN_SDL3VULKANGRAPHICSINSTANCE

#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "Engine/Platform/IWindow.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    class ENGINE_EXPORT SDL3VulkanGraphicsBridge : public IVulkanGraphicsBridge
    {
    public:
        SDL3VulkanGraphicsBridge() : IVulkanGraphicsBridge(Platform::SDL) {};
        ~SDL3VulkanGraphicsBridge() override = default;

        VkSurfaceKHR CreateSurface(VkInstance instance, Ref<IWindow> window) override;
        void DestroySurface(VkInstance instance, VkSurfaceKHR surface) override;
         
        std::vector<const char*> GetInstanceExtensions() override;
    };
} // namespace Engine


#endif // PLATFORM_SDL3_VULKAN_SDL3VULKANGRAPHICSINSTANCE
