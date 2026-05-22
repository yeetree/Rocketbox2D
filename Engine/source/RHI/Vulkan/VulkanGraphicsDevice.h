#ifndef RHI_VULKAN_VULKANGRAPHICSDEVICE
#define RHI_VULKAN_VULKANGRAPHICSDEVICE

#include <vulkan/vulkan_raii.hpp>

#include "Engine/RHI/IGraphicsDevice.h"

#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "RHI/Vulkan/VulkanContext.h"

#include <vector>

namespace Engine
{
    class ENGINE_EXPORT VulkanGraphicsDevice : public IGraphicsDevice
    {
    private:
        Ref<IVulkanGraphicsBridge> m_GraphicsBridge;
        Ref<VulkanContext> m_Context;

    public:
        VulkanGraphicsDevice(Ref<IGraphicsBridge> bridge, Ref<IWindow> window);
        ~VulkanGraphicsDevice() override;

        Scope<ISwapchain> CreateSwapchain(const SwapchainDesc& desc) override;

        void OnDestroy() override;

    };
} // namespace Engine

#endif // RHI_VULKAN_VULKANGRAPHICSDEVICE
