#ifndef RENDERER_VULKAN_IVULKANGRAPHICSBRIDGE
#define RENDERER_VULKAN_IVULKANGRAPHICSBRIDGE

#include "Engine/Platform/IGraphicsBridge.h"

#include <vector>

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    // fwd
    class IWindow;

    class IVulkanGraphicsBridge : public IGraphicsBridge {
    public:
        IVulkanGraphicsBridge() = default;
        virtual ~IVulkanGraphicsBridge() = default;

        virtual VkSurfaceKHR CreateSurface(VkInstance instance, IWindow* window) = 0;
        virtual std::vector<const char*> GetRequiredExtensions() = 0;
    };
} // namespace Engine


#endif // RENDERER_VULKAN_IVULKANGRAPHICSBRIDGE
