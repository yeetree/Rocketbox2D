#ifndef RHI_VULKAN_RHI_VULKANGRAPHICSDEVICE
#define RHI_VULKAN_RHI_VULKANGRAPHICSDEVICE

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/IGraphicsDevice.h"

#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanResourceData.h"
#include "RHI/Vulkan/VulkanCommandBufferAllocator.h"

#include <vector>
#include <array>
#include <unordered_map>

#include <vulkan/vulkan_raii.hpp>

namespace Engine::RHI::Vulkan
{
    // Forward
    class IVulkanGraphicsBridge;
    class VulkanFrame;

    class ENGINE_EXPORT VulkanGraphicsDevice: public IGraphicsDevice
    {
    private:
        // Context
        Scope<IVulkanGraphicsBridge> m_Bridge;
        VulkanContext m_Context;

        // Frame pacing
        std::vector<Scope<VulkanFrame>> m_Frames;
        uint32_t m_FrameIndex;

        // Global frame submission info
        std::vector<vk::SubmitInfo> m_FrameSubmits;
        std::vector<vk::CommandBuffer> m_FrameCommandBuffers;
        std::vector<vk::Semaphore> m_FrameWaitSemaphores;
        std::vector<vk::Semaphore> m_FrameSignalSemaphores;
        std::vector<vk::PipelineStageFlags> m_FrameStageFlags;
        std::vector<SwapChainHandle> m_FrameSwapChainPresentations;

        // Immediate command buffers
        bool                       m_InImmediatePass        = false;
        vk::raii::CommandPool      m_ImmediatePool          = nullptr;
        Scope<VulkanCommandBuffer> m_ImmediateCommandBuffer = nullptr;
        vk::raii::Fence            m_ImmediateFence         = nullptr;

        // TODO: Vulkan: VulkanResourceManager: Wrap resource creation, destruction, & destruction queue
        // Resources
        std::unordered_map<uint32_t, VulkanBufferData> m_Buffers;
        std::unordered_map<uint32_t, VulkanTextureData> m_Textures;
        std::unordered_map<uint32_t, VulkanShaderData> m_Shaders;
        std::unordered_map<uint32_t, VulkanPipelineData> m_Pipelines;
        std::unordered_map<uint32_t, VulkanSwapChainData> m_SwapChains;

        // Deletion queue
        struct QueuedDestruction {
            enum class Type { Buffer, Texture, Shader, Pipeline, SwapChain };
            Type     type;
            uint32_t id;
            uint32_t framesRemaining;
        };

        std::vector<QueuedDestruction> m_DeletionQueue;

        void EnqueueDeletion(QueuedDestruction::Type type, uint32_t id);

        // Destroy
        void ImmediateDestroy(QueuedDestruction::Type type, uint32_t id);

        // Swapchain
        void RebuildSwapchain(VulkanSwapChainData& swapChainData);

        // Textures
        void CreateImageView(VulkanTextureData& textureData);
        void CreateSampler(VulkanTextureData& textureData);

    public:
        VulkanGraphicsDevice(Scope<IVulkanGraphicsBridge> bridge);
        ~VulkanGraphicsDevice() override;

        // Resource creation
        BufferHandle    CreateBuffer(const BufferDesc& desc) override;
        TextureHandle   CreateTexture(const TextureDesc& desc) override;
        ShaderHandle    CreateShader(const ShaderDesc& desc) override;
        PipelineHandle  CreatePipeline(const PipelineDesc& desc) override;
        SwapChainHandle CreateSwapChain(const SwapChainDesc& desc) override;

        // Resource destruction
        void DestroyBuffer(BufferHandle& buffer) override;
        void DestroyTexture(TextureHandle& texture) override;
        void DestroyShader(ShaderHandle& shader) override;
        void DestroyPipeline(PipelineHandle& pipeline) override;
        void DestroySwapChain(SwapChainHandle& swapchain) override;

        // Frame pacing
        void BeginFrame() override;
        void EndFrame() override;

        // Render passes
        // ICommandBuffer* BeginPass(TextureHandle renderTarget, Vec4 clearColor) override;
        ICommandBuffer* BeginPass(SwapChainHandle renderTarget, Vec4 clearColor, TextureHandle depthBuffer = {}) override;
        void EndPass(ICommandBuffer* cmd) override;

        // Immediate command buffer
        ICommandBuffer* BeginImmediate() override;
        void EndImmediate(ICommandBuffer* cmd) override; // Blocks until GPU is finished with work

        // Swapchain configuration
        void ResizeSwapChain(SwapChainHandle swapchain, uint32_t width, uint32_t height) override;  // Called on window resize events
        void SetSwapChainPresentMode(SwapChainHandle swapchain, PresentMode mode) override;

        // Destroy
        void OnDestroy() override; // Called when application attempts to exit gracefully

        // Public getters for VulkanClasses. These assert that the handle is valid. This will not return if the handle is invalid.
        VulkanContext& GetContext()      { return m_Context; }
        VulkanFrame*   GetCurrentFrame() { return m_Frames[m_FrameIndex].get(); }
        uint32_t       GetFrameIndex()   { return m_FrameIndex; }

        // Resources
        VulkanBufferData&    GetBufferData(BufferHandle buffer);
        VulkanTextureData&   GetTextureData(TextureHandle texture);
        VulkanShaderData&    GetShaderData(ShaderHandle shader);
        VulkanPipelineData&  GetPipelineData(PipelineHandle pipeline);
        VulkanSwapChainData& GetSwapChainData(SwapChainHandle swapchain);
    };

} // namespace Engine::RHI::Vulkan


#endif // RHI_VULKAN_RHI_VULKANGRAPHICSDEVICE
