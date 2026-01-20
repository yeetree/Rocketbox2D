#ifndef RENDERER_VULKAN_VULKANBUFFER
#define RENDERER_VULKAN_VULKANBUFFER

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IBuffer.h"

namespace Engine {
    struct VulkanBuffer : public IBuffer {
    public:
        VulkanBuffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const BufferDesc& desc);
        ~VulkanBuffer() override;

        // Layout
        vk::raii::Buffer m_Buffer = nullptr;
        vk::raii::DeviceMemory m_BufferMemory = nullptr;
    private:
        static vk::BufferUsageFlagBits GetVulkanBufferUsage(BufferType type);
        static uint32_t FindMemoryType(uint32_t typeFilter, vk::PhysicalDeviceMemoryProperties memProperties, vk::MemoryPropertyFlags properties);
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANBUFFER
