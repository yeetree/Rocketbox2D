#ifndef RHI_VULKAN_VULKANCOMMANDBUFFER
#define RHI_VULKAN_VULKANCOMMANDBUFFER

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/ICommandBuffer.h"

#include "RHI/Vulkan/VulkanContext.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    class ENGINE_EXPORT VulkanCommandBuffer : public ICommandBuffer
    {
    private:
        Ref<VulkanContext> m_Context;
        vk::raii::CommandBuffer m_CommandBuffer;

    public:
        VulkanCommandBuffer(Ref<VulkanContext> context, const vk::raii::CommandPool& commandPool);
        ~VulkanCommandBuffer() override = default;

        void Begin() override;
        void End() override;

        const vk::raii::CommandBuffer& GetCommandBuffer() const { return m_CommandBuffer; }
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANCOMMANDBUFFER
