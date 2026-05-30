#ifndef RHI_VULKAN_VULKANDYNAMICBUFFERALLOCATOR
#define RHI_VULKAN_VULKANDYNAMICBUFFERALLOCATOR

#include "engine_export.h"

#include "RHI/Vulkan/VulkanContext.h"

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

namespace Engine::RHI::Vulkan
{
    // Forward
    class VulkanContext;

    class ENGINE_EXPORT VulkanDynamicBufferAllocator
    {
    private:
        VulkanContext& m_Context;

        VmaAllocation m_Allocation = nullptr;
        vk::Buffer m_Buffer = nullptr;
        void* m_MappedData = nullptr;
        size_t m_CurrentOffset = 0;
        size_t m_TotalSize = 0;
        size_t m_Alignment = 0;

    public:
        VulkanDynamicBufferAllocator(VulkanContext& context, size_t totalSize, vk::BufferUsageFlags usage, size_t alignment);
        ~VulkanDynamicBufferAllocator();

        size_t AllocateAndCopy(void* data, size_t size);

        vk::Buffer& GetBuffer() { return m_Buffer; }

        void Reset();
    };
} // namespace Engine::RHI::Vulkan


#endif // RHI_VULKAN_VULKANDYNAMICBUFFERALLOCATOR
