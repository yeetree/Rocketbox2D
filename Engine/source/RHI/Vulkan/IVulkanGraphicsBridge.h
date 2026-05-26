#ifndef RHI_VULKAN_IVULKANGRAPHICSBRIDGE
#define RHI_VULKAN_IVULKANGRAPHICSBRIDGE

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Platform/IGraphicsBridge.h"
#include "Engine/Platform/IWindow.h"

#include <vector>

namespace Engine
{
    class ENGINE_EXPORT IVulkanGraphicsBridge : public IGraphicsBridge
    {
    protected:
        IVulkanGraphicsBridge(Platform platform) : IGraphicsBridge(platform, GraphicsAPI::Vulkan) {};

    public:
        virtual ~IVulkanGraphicsBridge() = default;

        // Creates a surface from an IWindow.
        // Recieves current presentQueueIndex to make sure that this surface is okay for this presention queue
        virtual VkSurfaceKHR CreateSurface(VkInstance instance, VkPhysicalDevice pd, uint32_t presentQueueIndex, IWindow* window) = 0;
        
        // virtual void DestroySurface(VkInstance instance, VkSurfaceKHR surface) = 0;
        // Not needed. When wrapped into a vk::raii::SurfaceKHR with an instance,
        // destruction is automatic. I'm leaving it here just in case...

        // Create/Destroy dummy surfaces for physical device selection
        // Spawns an invisible window and uses it to query for presentation support
        virtual VkSurfaceKHR* CreateDummySurface(VkInstance instance) = 0;
        virtual void DestroyDummySurface(VkInstance instance) = 0;

        virtual std::vector<const char*> GetInstanceExtensions() = 0;
    };
} // namespace Engine


#endif // RHI_VULKAN_IVULKANGRAPHICSBRIDGE
