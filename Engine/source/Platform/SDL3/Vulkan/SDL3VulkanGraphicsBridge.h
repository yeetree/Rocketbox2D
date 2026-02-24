#ifndef PLATFORM_SDL3_VULKAN_SDL3VULKANGRAPHICSBRIDGE
#define PLATFORM_SDL3_VULKAN_SDL3VULKANGRAPHICSBRIDGE

#include "Renderer/Vulkan/IVulkanGraphicsBridge.h"

namespace Engine {
    class SDL3VulkanGraphicsBridge : public IVulkanGraphicsBridge {
    public:
        SDL3VulkanGraphicsBridge() = default;
        ~SDL3VulkanGraphicsBridge() override = default;

        VkSurfaceKHR CreateSurface(VkInstance instance, IWindow* window) override;
        std::vector<const char*> GetRequiredExtensions() override;
    };
} // namespace Engine

#endif // PLATFORM_SDL3_VULKAN_SDL3VULKANGRAPHICSBRIDGE
