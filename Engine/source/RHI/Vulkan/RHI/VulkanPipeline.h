#ifndef RHI_VULKAN_RHI_VULKANPIPELINE
#define RHI_VULKAN_RHI_VULKANPIPELINE

#include "Engine/RHI/IPipeline.h"

#include "RHI/Vulkan/VulkanContext.h"

namespace Engine
{
    class ENGINE_EXPORT VulkanPipeline : public IPipeline
    {
    public:
        VulkanPipeline(VulkanContext* context, const PipelineDesc& desc);
        ~VulkanPipeline() override = default;
    };
} // namespace Engine


#endif // RHI_VULKAN_RHI_VULKANPIPELINE
