#ifndef RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR
#define RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR

#include "engine_export.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine::RHI::Vulkan
{
    // Forward
    class VulkanContext;

    class ENGINE_EXPORT VulkanDescriptorSetAllocator
    {
    private:
        VulkanContext& m_Context;
        vk::raii::DescriptorPool m_DescriptorPool = nullptr;

    public:
        VulkanDescriptorSetAllocator(VulkanContext& context);

        // Returns existing or allocates new set for this pipeline
        // TODO: Vulkan: Rewrite VulkanDescriptorSetAllocator;
        // vk::DescriptorSet GetOrAllocate(VulkanPipeline* pipeline);

        void Reset();    
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR
