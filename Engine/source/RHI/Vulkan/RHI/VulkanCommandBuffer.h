#ifndef RHI_VULKAN_RHI_VULKANCOMMANDBUFFER
#define RHI_VULKAN_RHI_VULKANCOMMANDBUFFER

#include "Engine/RHI/ICommandBuffer.h"

#include "Engine/Math/Vector.h"

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

namespace Engine::RHI::Vulkan
{
    // Forward
    class VulkanGraphicsDevice;
    class VulkanTextureData;

    class ENGINE_EXPORT VulkanCommandBuffer : public ICommandBuffer
    {
    private:
        friend class VulkanGraphicsDevice;

        // Vulkan
        VulkanGraphicsDevice& m_GraphicsDevice;
        vk::raii::CommandBuffer m_CommandBuffer = nullptr;

        // State
        VulkanTextureData*  m_CurrentRenderTarget = nullptr;
        PipelineHandle      m_BoundPipelineHandle;

        // Staging buffer
        struct StagingBufferAllocation {
            VkBuffer buffer;
            VmaAllocation allocation;
        };
        std::vector<StagingBufferAllocation> m_StagingBufferAllocations;

        // Begin/End* for Vulkan classes
        void BeginRendering(VulkanTextureData* renderTarget, Vec4 clearColor);
        void EndRendering();
        void BeginImmediate();
        void EndImmediate();

    public:
        VulkanCommandBuffer(VulkanGraphicsDevice& graphicsDevice, vk::CommandPool commandPool);
        ~VulkanCommandBuffer() override = default;

        // Graphics
        void BindPipeline(PipelineHandle pipeline) override;
        void BindVertexBuffer(BufferHandle buffer) override;
        void BindIndexBuffer(BufferHandle buffer) override;
        void BindUniformBuffer(BufferHandle buffer, uint32_t binding) override;
        void BindTexture(TextureHandle texture, uint32_t binding) override;
        void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t indexOffset = 0, uint32_t firstInstance = 0) override;
   
        // Data
        void UploadBuffer(BufferHandle buffer, void* data, size_t size, size_t offset) override;
        void UploadTexture(TextureHandle texture, void* data) override;

        // Public getters for Vulkan classes
        vk::raii::CommandBuffer& GetCommandBuffer() { return m_CommandBuffer; }

        // Resetter for Vulkan classes
        void Reset();
    };
}

#endif // RHI_VULKAN_RHI_VULKANCOMMANDBUFFER
