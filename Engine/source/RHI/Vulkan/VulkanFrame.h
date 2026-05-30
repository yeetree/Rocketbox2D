#ifndef RHI_VULKAN_VULKANFRAME
#define RHI_VULKAN_VULKANFRAME

#include "engine_export.h"

#include "RHI/Vulkan/VulkanCommandBufferAllocator.h"
#include "RHI/Vulkan/VulkanDynamicBufferAllocator.h"
#include "RHI/Vulkan/VulkanDescriptorSetAllocator.h"

#include <cstdint>

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

namespace Engine::RHI::Vulkan
{
    // VulkanFrame manages synchronization and command buffer acquisition in frames in flight.
    class ENGINE_EXPORT VulkanFrame
    {
    private:
        // CPU Fence
        vk::raii::Fence m_Fence = nullptr;

        // Comand buffer allocation
        VulkanCommandBufferAllocator m_CommandBufferAllocator;

        // Dynamic buffers
        VulkanDynamicBufferAllocator m_VertexDynamicBufferAllocator;
        VulkanDynamicBufferAllocator m_IndexDynamicBufferAllocator;
        VulkanDynamicBufferAllocator m_UniformDynamicBufferAllocator;

        // Descriptor set allocator
        VulkanDescriptorSetAllocator m_DescriptorSetAllocator;

    public:
        VulkanFrame(VulkanContext& context);

        // Begin new frame
        void Reset();

        // Getters
        vk::raii::Fence& GetFence() { return m_Fence; }
        VulkanCommandBufferAllocator& GetCommandBufferAllocator() { return m_CommandBufferAllocator; };
        VulkanDynamicBufferAllocator& GetVertexDynamicBufferAllocator() { return m_VertexDynamicBufferAllocator; }
        VulkanDynamicBufferAllocator& GetIndexDynamicBufferAllocator() { return m_IndexDynamicBufferAllocator; }
        VulkanDynamicBufferAllocator& GetUniformDynamicBufferAllocator() { return m_UniformDynamicBufferAllocator; }
        VulkanDescriptorSetAllocator& GetDescriptorSetAllocator() { return m_DescriptorSetAllocator; }

    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANFRAME
