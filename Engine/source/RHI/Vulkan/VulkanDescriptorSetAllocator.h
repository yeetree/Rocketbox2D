#ifndef RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR
#define RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR

#include "engine_export.h"

#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/RHI/VulkanPipeline.h"

#include <unordered_map>
#include <cstdint>

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    class VulkanDescriptorSetAllocator
    {
    private:
        VulkanContext* m_Context = nullptr;
        vk::raii::DescriptorPool m_DescriptorPool = nullptr;
        std::unordered_map<uint32_t, vk::raii::DescriptorSet> m_Sets; // indexed by pipeline ID

    public:
        VulkanDescriptorSetAllocator(VulkanContext* context);

        // Returns existing or allocates new set for this pipeline
        vk::DescriptorSet GetOrAllocate(VulkanPipeline* pipeline);

        void Reset();    
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR
