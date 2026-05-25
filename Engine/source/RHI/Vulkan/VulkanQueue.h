#ifndef RHI_VULKAN_VULKANQUEUE
#define RHI_VULKAN_VULKANQUEUE

#include "engine_export.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    struct ENGINE_EXPORT VulkanQueue
    {
        VulkanQueue(vk::raii::Queue q, uint32_t i) : queue(q), familyIndex(i) {}
        VulkanQueue() = default;
        vk::raii::Queue queue = nullptr;
        uint32_t familyIndex = -1;
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANQUEUE
