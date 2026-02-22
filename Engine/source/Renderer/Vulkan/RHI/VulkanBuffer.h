#ifndef RENDERER_VULKAN_RHI_VULKANBUFFER
#define RENDERER_VULKAN_RHI_VULKANBUFFER

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
        size_t GetSize() const override;

        // Public getters for VulkanGraphicsDevice
        VkBuffer& GetBuffer();

    private:
        // Vulkan members
        struct BufferInfo {
            VkBuffer buffer;
            VmaAllocation allocation;
            void* mapPointer;
        };
        std::vector<BufferInfo> m_Buffers;
        bool m_IsDynamic;

        VulkanGraphicsDevice* m_GraphicsDevice;
        size_t m_Size;

        static VkBufferUsageFlags GetVulkanBufferUsage(BufferType type);
    };
} // namespace Engine

#endif // RENDERER_VULKAN_RHI_VULKANBUFFER
