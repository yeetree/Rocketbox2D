#ifndef RHI_VULKAN_RHI_VULKANGRAPHICSDEVICE
#define RHI_VULKAN_RHI_VULKANGRAPHICSDEVICE

#include "Engine/RHI/IGraphicsDevice.h"

#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "RHI/Vulkan/RHI/VulkanSwapChain.h"

#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanFrame.h"
#include "RHI/Vulkan/VulkanCommandBufferPool.h"

namespace Engine
{
    class ENGINE_EXPORT VulkanGraphicsDevice : public IGraphicsDevice
    {
    private:
        // Vulkan helper classes
        Scope<IVulkanGraphicsBridge> m_Bridge;
        Scope<VulkanContext> m_Context;

        // Single-time command pool
        vk::raii::CommandPool m_SingleCommandPool = nullptr;

        // Frame info
        std::vector<Scope<VulkanFrame>> m_Frames;
        uint32_t m_FrameIndex = 0;

        // Global frame submission info
        std::vector<vk::SubmitInfo> m_FrameSubmits;
        std::vector<vk::CommandBuffer> m_FrameCommandBuffers;
        std::vector<vk::Semaphore> m_FrameWaitSemaphores;
        std::vector<vk::Semaphore> m_FrameSignalSemaphores;
        std::vector<vk::PipelineStageFlags> m_FrameStageFlags;
        std::vector<VulkanSwapChain*> m_FrameSwapChainPresentations;

    public:
        VulkanGraphicsDevice(Scope<IVulkanGraphicsBridge> bridge);
        ~VulkanGraphicsDevice() = default;

        Scope<ISwapChain> CreateSwapChain(const SwapChainDesc& desc) override;
        Scope<IShader> CreateShader(const ShaderDesc& desc) override;
        Scope<IPipeline> CreatePipeline(const PipelineDesc& desc) override;
        Scope<IBuffer> CreateBuffer(const BufferDesc& desc) override;

        // Frame pacing
        void BeginFrame() override;
        void EndFrame() override;

        // Single time commands
        ICommandBuffer* BeginSingleTimeCommands() override;
        void EndSingleTimeCommands(ICommandBuffer* cmd) override; // Blocks until completion

        // Swapchain passees
        ICommandBuffer* BeginSwapChainPass(ISwapChain* swapchain) override;
        void EndSwapChainPass(ISwapChain* swapchain, ICommandBuffer* cmd) override;

        // Swapchain config
        void ResizeSwapChain(ISwapChain* swapchain, uint32_t width, uint32_t height) override; // Called on window resize events
        void SetSwapChainPresentation(ISwapChain* swapchain, PresentMode presentation) override;

        // Dynamic buffers
        void SetBufferData(IBuffer* buffer, void* data, size_t size) override;

        void OnDestroy() override;
    };
} // namespace Engine


#endif // RHI_VULKAN_RHI_VULKANGRAPHICSDEVICE
