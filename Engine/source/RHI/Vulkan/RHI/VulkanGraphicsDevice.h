#ifndef RHI_VULKAN_VULKANGRAPHICSDEVICE
#define RHI_VULKAN_VULKANGRAPHICSDEVICE

#include "Engine/RHI/IGraphicsDevice.h"

#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"

namespace Engine
{
    class ENGINE_EXPORT VulkanGraphicsDevice : public IGraphicsDevice
    {
    private:
        
        // Frame pacing
        uint32_t m_FrameIndex;
        std::vector<vk::raii::Fence> m_Fences;

        Scope<IVulkanGraphicsBridge> m_Bridge;
        Scope<VulkanContext> m_Context;

        // Temporary
        std::vector<std::vector<Scope<VulkanCommandBuffer>>> m_AllocatedCommandBuffers;

    public:
        VulkanGraphicsDevice(Scope<IVulkanGraphicsBridge> bridge);
        ~VulkanGraphicsDevice() = default;

        Scope<ISwapChain> CreateSwapChain(const SwapChainDesc& desc) override;

        // Frame pacing
        void BeginFrame() override;
        void EndFrame() override;

        // Swapchain passees
        ICommandBuffer* BeginSwapChainPass(Ref<ISwapChain> swapchain) override;
        void EndSwapChainPass(Ref<ISwapChain> swapchain, ICommandBuffer* cmd) override;

        void OnDestroy() override;
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANGRAPHICSDEVICE
