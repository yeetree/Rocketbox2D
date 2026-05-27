#ifndef RHI_VULKAN_RHI_VULKANBUFFER
#define RHI_VULKAN_RHI_VULKANBUFFER

#include "Engine/RHI/IBuffer.h"

#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanFrame.h"

namespace Engine
{
    // Forward
    class VulkanGraphicsDevice;

    class ENGINE_EXPORT VulkanBuffer : public IBuffer
    {
    private:
        // For dynamic buffers:
        // All dynamic buffers are suballocations of a global dynamic mega buffer.
        // This is indexed per global frame and returns the byte offset of this buffer
        // in the mega buffer.
        std::vector<size_t> m_DynamicOffsets;

        // For static buffers:
        VmaAllocator m_Allocator = nullptr;
        vk::Buffer m_Buffer = nullptr;
        VmaAllocation m_Allocation = nullptr;

    public:
        VulkanBuffer(VulkanContext* context, const BufferDesc& desc);
        ~VulkanBuffer();

        // Vulkan

        // Returns current dynamic buffer offset or 0 for static
        size_t GetOffset(uint32_t frameIdx); 

        // Static only: Get buffer
        vk::Buffer& GetStaticBuffer() { return m_Buffer; };

        // Dynamic only: Set data
        void SetData(VulkanFrame* frame, uint32_t frameIdx, void* data, size_t size);

        // Helpers
        static vk::BufferUsageFlags GetBufferUsageFlags(BufferType type);
    };
} // namespace Engine


#endif // RHI_VULKAN_RHI_VULKANBUFFER
