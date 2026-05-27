#ifndef RHI_VULKAN_VULKANFRAME
#define RHI_VULKAN_VULKANFRAME

#include "engine_export.h"

#include "Engine/RHI/IBuffer.h"

#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanDynamicBuffer.h"
#include "RHI/Vulkan/VulkanCommandBufferPool.h"
#include "RHI/Vulkan/VulkanDescriptorSetAllocator.h"

#include <cstdint>

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

namespace Engine
{
    // VulkanFrame manages synchronization and command buffer acquisition in frames in flight.
    class ENGINE_EXPORT VulkanFrame
    {
    private:
        // CPU Fence
        vk::raii::Fence m_Fence = nullptr;

        // Comand buffer allocation
        Scope<VulkanCommandBufferPool> m_CommandBufferPool;

        // Dynamic buffers
        Scope<VulkanDynamicBuffer> m_VertexDynamicBuffer;
        Scope<VulkanDynamicBuffer> m_IndexDynamicBuffer;
        Scope<VulkanDynamicBuffer> m_UniformDynamicBuffer;

        // Descriptor set allocator
        Scope<VulkanDescriptorSetAllocator> m_DescriptorSetAllocator = nullptr;

    public:
        VulkanFrame(VulkanContext* context);

        // Begin new frame
        void Reset();

        // Getters
        VulkanCommandBufferPool* GetCommandBufferPool() { return m_CommandBufferPool.get(); };
        VulkanDynamicBuffer* GetDynamicBuffer(BufferType type);
        vk::raii::Fence& GetFence() { return m_Fence; }
        VulkanDescriptorSetAllocator* GetDescriptorSetAllocator() { return m_DescriptorSetAllocator.get(); }

    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANFRAME
