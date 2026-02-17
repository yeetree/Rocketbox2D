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

namespace Engine {
    // Fwd:
    class VulkanBuffer;

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
        Scope<ITexture> CreateTexture(const TextureDesc& desc) override;
        Scope<IShader> CreateShader(const ShaderDesc& desc) override;
        Scope<IPipelineState> CreatePipelineState(const PipelineDesc& desc) override;
        Scope<IVertexArray> CreateVertexArray(const VertexArrayDesc& desc) override;

        // Frame Management
        void BeginFrame() override;
        void EndFrame() override;
        void Present() override;
        void SetClearColor(Vec4 color) override;

        // Draw call
        void SubmitDraw(IBuffer& vbo, IBuffer& ebo, IPipelineState& pipeline, uint32_t indexCount) override;

        // Push constants
        void PushConstants(IPipelineState& pipeline, const void* data, uint32_t size) override;

        // Resize
        void Resize(int width, int height) override;

        // Vulkan-Specific

        // Helper function for VulkanBuffer
        void StageBufferUploadData(VulkanBuffer* dstBuffer, const void* data, size_t size, size_t dstOffset);

        
        // Public So Vulkan* classes can access them
        Scope<VulkanContext> m_Context;
        Scope<VulkanDevice> m_Device;
        Scope<VulkanSwapchain> m_Swapchain;
    
        uint32_t m_FrameIndex;
        uint32_t m_ImageIndex;

    private:
        // Gives GraphicsDevice chance to finish work before app can destroy
        void OnDestroy() override;

        // Utility
        [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code) const;
        void TransitionImageLayout(
            vk::raii::CommandBuffer& cmd,
            uint32_t imageIndex,
            vk::ImageLayout         old_layout,
            vk::ImageLayout         new_layout,
            vk::AccessFlags2        src_access_mask,
            vk::AccessFlags2        dst_access_mask,
            vk::PipelineStageFlags2 src_stage_mask,
            vk::PipelineStageFlags2 dst_stage_mask);
    };

} // namespace Engine

#endif // RENDERER_VULKAN_VULKANGRAPHICSDEVICE
