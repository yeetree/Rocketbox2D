#ifndef RHI_VULKAN_VULKANFRAME
#define RHI_VULKAN_VULKANFRAME

#include "engine_export.h"

#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"

#include <cstdint>

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    // VulkanFrame manages synchronization and command buffer acquisition in frames in flight.
    class ENGINE_EXPORT VulkanFrame
    {
    private:
        vk::raii::Fence m_Fence = nullptr;

        vk::raii::CommandPool m_CommandPool = nullptr;
        std::vector<Scope<VulkanCommandBuffer>> m_CommandBuffers;

        uint32_t m_UsedCommandBuffers = 0;

    public:
        VulkanFrame(VulkanContext* context);

        // Begin new frame
        void Reset();

        // Returns reused or newly allocated command buffer
        VulkanCommandBuffer* GetCommandBuffer(VulkanContext* context);

        // Returns fence
        vk::raii::Fence& GetFence() { return m_Fence; }

    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANFRAME
