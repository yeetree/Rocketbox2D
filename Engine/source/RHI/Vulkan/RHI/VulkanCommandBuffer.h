#ifndef RHI_VULKAN_RHI_VULKANCOMMANDBUFFER
#define RHI_VULKAN_RHI_VULKANCOMMANDBUFFER

#include "Engine/RHI/ICommandBuffer.h"

#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanFrame.h"
#include "RHI/Vulkan/RHI/VulkanBuffer.h"
#include "RHI/Vulkan/RHI/VulkanTexture.h"

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

namespace Engine
{
    class ENGINE_EXPORT VulkanCommandBuffer : public ICommandBuffer
    {
    private:
        // Command Buffer
        vk::raii::CommandBuffer m_CommandBuffer = nullptr;    

        // Allocator (for staging)
        VmaAllocator m_Allocator = nullptr;

        // Staging
        struct StagingBufferAllocation {
            VkBuffer buffer;
            VmaAllocation allocation;
        };
        std::vector<StagingBufferAllocation> m_StagingBufferAllocations;

        // Frame info
        uint32_t m_FrameIndex = -1;
        VulkanFrame* m_Frame = nullptr;

        // Helpers
        vk::Buffer GetVulkanBuffer(VulkanBuffer* buffer);

    public:
        VulkanCommandBuffer(VulkanContext* context, vk::CommandPool pool);
        ~VulkanCommandBuffer() = default;

        void Begin() override;
        void End() override;

        // Render target
        void BeginRendering(ITexture* renderTarget, Vec4 clearColor) override;
        void EndRendering(ITexture* renderTarget) override;

        // Graphics
        void BindPipeline(IPipeline* pipeline) override;
        void BindVertexBuffer(IBuffer* buffer) override;
        void BindIndexBuffer(IBuffer* buffer) override;
        void BindUniformBuffer(IBuffer* buffer, IPipeline* pipeline, uint32_t binding) override;
        void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t indexOffset = 0, uint32_t firstInstance = 0) override;

        // Data
        void SetBufferData(IBuffer* buffer, void* data, size_t size, size_t offset) override;

        // Vulkan
        void SetFrameInfo(uint32_t frameIdx, VulkanFrame* frame);
        void FreeStagingBufferAllocations();
        vk::raii::CommandBuffer& GetCommandBuffer() { return m_CommandBuffer; }
        
    };
} // namespace Engine


#endif // RHI_VULKAN_RHI_VULKANCOMMANDBUFFER
