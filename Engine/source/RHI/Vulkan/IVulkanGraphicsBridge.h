#ifndef RHI_VULKAN_IVULKANGRAPHICSBRIDGE
#define RHI_VULKAN_IVULKANGRAPHICSBRIDGE

#include "Engine/Platform/IGraphicsBridge.h"

#include <vector>

#include <vulkan/vulkan_raii.hpp>

// Forward
namespace Engine
{
    class IWindow;
}

namespace Engine::RHI::Vulkan
{
    class ENGINE_EXPORT IVulkanGraphicsBridge : public IGraphicsBridge
    {
    protected:
        IVulkanGraphicsBridge(Platform platform) : IGraphicsBridge(platform, GraphicsAPI::Vulkan) {};

    public:
        virtual ~IVulkanGraphicsBridge() = default;

        // Creates a surface from an IWindow.
        // Recieves current presentQueueIndex to make sure that this surface is okay for this presention queue
        virtual vk::SurfaceKHR CreateSurface(vk::Instance instance, vk::PhysicalDevice pd, uint32_t presentQueueIndex, Engine::IWindow* window) = 0;
        
        // virtual void DestroySurface(VkInstance instance, VkSurfaceKHR surface) = 0;
        // Not needed. When wrapped into a vk::raii::SurfaceKHR with an instance,
        // destruction is automatic. I'm leaving it here just in case...

        // Create/Destroy dummy surfaces for physical device selection
        // Spawns an invisible window and uses it to query for presentation support
        virtual vk::SurfaceKHR* CreateDummySurface(vk::Instance instance) = 0;
        virtual void DestroyDummySurface(vk::Instance instance) = 0;

        // Returns required instance extensions provided by the platform
        virtual std::vector<const char*> GetInstanceExtensions() = 0;
    };
} // namespace Engine::RHI::Vulkan


#endif // RHI_VULKAN_IVULKANGRAPHICSBRIDGE
