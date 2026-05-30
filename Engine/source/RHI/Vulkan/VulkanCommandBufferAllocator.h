#ifndef RHI_VULKAN_VULKANCOMMANDBUFFERALLOCATOR
#define RHI_VULKAN_VULKANCOMMANDBUFFERALLOCATOR

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include <vector>
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>

namespace Engine::RHI::Vulkan
{
    // Forward
    class VulkanContext;
    class VulkanGraphicsDevice;
    class VulkanCommandBuffer;

    class ENGINE_EXPORT VulkanCommandBufferAllocator
    {
    private:
        VulkanContext& m_Context;
        vk::raii::CommandPool m_CommandPool = nullptr;
        std::vector<Scope<VulkanCommandBuffer>> m_CommandBuffers;
        uint32_t m_UsedCommandBuffers = 0;

    public:
        VulkanCommandBufferAllocator(VulkanContext& context);
        ~VulkanCommandBufferAllocator(); 

        VulkanCommandBuffer* GetOrAllocate(VulkanGraphicsDevice& graphicsDevice);

        void Reset();
    };
} // namespace Engine::RHI::Vulkan


#endif // RHI_VULKAN_VULKANCOMMANDBUFFERALLOCATOR
