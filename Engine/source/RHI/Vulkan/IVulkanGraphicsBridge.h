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

        virtual VkSurfaceKHR CreateSurface(VkInstance instance, Ref<IWindow> window) = 0;
        virtual void DestroySurface(VkInstance instance, VkSurfaceKHR surface) = 0;

        virtual std::vector<const char*> GetInstanceExtensions() = 0;
    };
} // namespace Engine


#endif // RHI_VULKAN_IVULKANGRAPHICSBRIDGE
