#ifndef PLATFORM_SDL3_VULKAN_SDL3VULKANGRAPHICSBRIDGE
#define PLATFORM_SDL3_VULKAN_SDL3VULKANGRAPHICSBRIDGE

#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "Engine/Platform/IWindow.h"

#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    class ENGINE_EXPORT SDL3VulkanGraphicsBridge : public IVulkanGraphicsBridge
    {
    private:
        SDL_Window* m_DummyWindow = nullptr;
        VkSurfaceKHR m_DummySurface = nullptr;

    public:
        SDL3VulkanGraphicsBridge() : IVulkanGraphicsBridge(Platform::SDL) {};
        ~SDL3VulkanGraphicsBridge() override = default;

        VkSurfaceKHR CreateSurface(VkInstance instance, VkPhysicalDevice pd, uint32_t presentQueueIndex, IWindow* window) override;
        // void DestroySurface(VkInstance instance, VkSurfaceKHR surface) override;
        // please kindy see source/RHI/Vulkan/IVulkanGraphicsBridge.h

        // Create/Destroy dummy surfaces for physical device selection
        // Spawns an invisible window and uses it to query for presentation support
        VkSurfaceKHR* CreateDummySurface(VkInstance instance) override;
        void DestroyDummySurface(VkInstance instance) override;
         
        std::vector<const char*> GetInstanceExtensions() override;
    };
} // namespace Engine


#endif // PLATFORM_SDL3_VULKAN_SDL3VULKANGRAPHICSBRIDGE
