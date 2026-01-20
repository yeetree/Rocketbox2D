#ifndef RENDERER_VULKAN_VULKANBUFFER
#define RENDERER_VULKAN_VULKANBUFFER

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IBuffer.h"

namespace Engine {
    class VulkanGraphicsDevice;

    struct VulkanBuffer : public IBuffer {
    public:
        VulkanBuffer(VulkanGraphicsDevice* graphicsDevice, const BufferDesc& desc);
        ~VulkanBuffer() override;

        void UpdateData(const void* data, size_t size, size_t offset) override;

        // Buffer
        vk::raii::Buffer m_Buffer = nullptr;
        vk::raii::DeviceMemory m_BufferMemory = nullptr;
        vk::DeviceSize m_BufferSize; // Buffer size
        BufferType m_Type;

    private:
        VulkanGraphicsDevice* m_GraphicsDevice;

        bool m_IsHostVisible; // Dynamic or static

        // Mapped memory pointer for dynamic
        void* m_MappedPtr = nullptr;

        static vk::Flags<vk::BufferUsageFlagBits> GetVulkanBufferUsage(BufferType type);
        static uint32_t FindMemoryType(uint32_t typeFilter, vk::PhysicalDeviceMemoryProperties memProperties, vk::MemoryPropertyFlags properties);
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANBUFFER
