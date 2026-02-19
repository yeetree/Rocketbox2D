#ifndef RENDERER_VULKAN_VULKANGRAPHICSDEVICE
#define RENDERER_VULKAN_VULKANGRAPHICSDEVICE

#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>

#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanSwapchain.h"

#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/IPipelineState.h"
#include "Engine/Renderer/RHI/IUniformBuffer.h"

namespace Engine {
    // Fwd:
    class VulkanBuffer;
    class VulkanUniformBuffer;
    class VulkanPipelineState;

    // Vulkan Implementation of IGraphicsDevice
    class VulkanGraphicsDevice : public IGraphicsDevice {
    public:
        // Constructor: Creates an VulkanGraphicsDevice with SDL_Window
        VulkanGraphicsDevice(SDL_Window* window);
        ~VulkanGraphicsDevice() override;

        // Prevent copying
        VulkanGraphicsDevice(const VulkanGraphicsDevice&) = delete;
        VulkanGraphicsDevice& operator=(const VulkanGraphicsDevice&) = delete;

        // Resource Creation
        Scope<IBuffer> CreateBuffer(const BufferDesc& desc) override;
        Scope<IUniformBuffer> CreateUniformBuffer(const UniformBufferDesc& desc) override;
        Scope<ITexture> CreateTexture(const TextureDesc& desc) override;
        Scope<IShader> CreateShader(const ShaderDesc& desc) override;
        Scope<IPipelineState> CreatePipelineState(const PipelineDesc& desc) override;
        Scope<IVertexArray> CreateVertexArray(const VertexArrayDesc& desc) override;

        // Frame Management
        void BeginFrame() override;
        void EndFrame() override;
        void Present() override;
        void SetClearColor(Vec4 color) override;

        // Bind Pipeline state
        void BindPipelineState(IPipelineState& pipeline) override;
        void SubmitDraw(IBuffer& vbo, IBuffer& ebo, uint32_t indexCount) override;
        void PushConstants(const void* data, uint32_t size) override;
        void BindUniformBuffer(IUniformBuffer& buffer, uint32_t binding) override;
        void BindTexture(ITexture& texture, uint32_t slot) override;

        // Resize
        void Resize(int width, int height) override;

        // Vulkan-Specific

        // Public helper function to begin/end one time command buffers
        vk::raii::CommandBuffer BeginOneTimeCommands();
        void EndOneTimeCommands(vk::raii::CommandBuffer& commandBuffer);

        // Public helper function for transitioning image layouts
        void TransitionImageLayout(
            vk::raii::CommandBuffer& cmd,
            vk::Image               image, // Take raw image handle
            vk::ImageLayout         oldLayout,
            vk::ImageLayout         newLayout,
            vk::AccessFlags2        srcAccess,
            vk::AccessFlags2        dstAccess,
            vk::PipelineStageFlags2 srcStage,
            vk::PipelineStageFlags2 dstStage);

        // Getters for Vulkan* classes (Not declared in IGraphicsDevice)
        uint32_t GetFrameIndex();
        vk::raii::DescriptorPool& GetDescriptorPool();
        vk::DescriptorSetLayout GetUBODescriptorSetLayout();
        vk::DescriptorSetLayout GetTextureDescriptorSetLayout();
        VulkanContext& GetContext();
        VulkanDevice& GetDevice();
        VulkanSwapchain& GetSwapchain();

    private:
        // Gives GraphicsDevice chance to finish work before app can destroy
        void OnDestroy() override;

        // Vulkan state
        Scope<VulkanContext> m_Context;
        Scope<VulkanDevice> m_Device;
        Scope<VulkanSwapchain> m_Swapchain;

        // Frame info
        uint32_t m_FrameIndex;
        uint32_t m_ImageIndex;

        // Descriptors
        vk::raii::DescriptorPool m_DescriptorPool = nullptr;
        vk::raii::DescriptorSetLayout m_UBOLayout = nullptr;
        vk::raii::DescriptorSetLayout m_TextureLayout = nullptr;

        // Current PSO
        VulkanPipelineState* m_CurrentPipelineState;

        // Current clear color
        vk::ClearColorValue m_ClearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    };

} // namespace Engine

#endif // RENDERER_VULKAN_VULKANGRAPHICSDEVICE
