#ifndef RENDERER_VULKAN_VULKANUNIFORMBUFFER
#define RENDERER_VULKAN_VULKANUNIFORMBUFFER

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/IUniformBuffer.h"

namespace Engine {
    // fwd
    class VulkanGraphicsDevice;
    class VulkanBuffer;

    class VulkanUniformBuffer : public IUniformBuffer {
    public:
        VulkanUniformBuffer(VulkanGraphicsDevice* graphicsDevice, const UniformBufferDesc& desc);
        ~VulkanUniformBuffer() override;

        void UpdateData(const void* data, size_t size, size_t offset) override;

        // Public getters for VulkanGraphicsDevice
        // (Not declared in IUniformBuffer)
        VulkanBuffer& GetBuffer(uint32_t index);
        vk::DescriptorSet GetDescriptorSet(uint32_t frameIndex);

    private:
        std::vector<Scope<VulkanBuffer>> m_Buffers;
        std::vector<vk::raii::DescriptorSet> m_DescriptorSets;
        VulkanGraphicsDevice* m_GraphicsDevice;
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANUNIFORMBUFFER
