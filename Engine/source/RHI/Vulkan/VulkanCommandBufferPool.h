#ifndef RHI_VULKAN_VULKANCOMMANDBUFFERPOOL
#define RHI_VULKAN_VULKANCOMMANDBUFFERPOOL

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "RHI/Vulkan/VulkanContext.h"

#include <vector>
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    // Forward
    class VulkanCommandBuffer;

    class ENGINE_EXPORT VulkanCommandBufferPool
    {
    private:
        vk::raii::CommandPool m_CommandPool = nullptr;
        std::vector<Scope<VulkanCommandBuffer>> m_CommandBuffers;
        uint32_t m_UsedCommandBuffers = 0;

    public:
        VulkanCommandBufferPool(VulkanContext* context);

        VulkanCommandBuffer* GetCommandBuffer(VulkanContext* context);

        void Reset();
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANCOMMANDBUFFERPOOL
