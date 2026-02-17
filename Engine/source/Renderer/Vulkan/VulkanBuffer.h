#ifndef RENDERER_VULKAN_VULKANBUFFER
#define RENDERER_VULKAN_VULKANBUFFER

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

#include "Engine/Renderer/RHI/IBuffer.h"

namespace Engine {
    // fwd
    class VulkanGraphicsDevice;

    class VulkanBuffer : public IBuffer {
    public:
        VulkanBuffer(VulkanGraphicsDevice* graphicsDevice, const BufferDesc& desc);
        ~VulkanBuffer() override;

        void UpdateData(const void* data, size_t size, size_t offset) override;

        // Handles
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;

    private:
        VulkanGraphicsDevice* m_GraphicsDevice;
        bool m_IsHostVisible;
        void* m_MappedPtr = nullptr;

        static VkBufferUsageFlags GetVulkanBufferUsage(BufferType type);
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANBUFFER
