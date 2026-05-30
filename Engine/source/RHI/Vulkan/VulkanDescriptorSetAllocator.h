#ifndef RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR
#define RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR

#include "engine_export.h"

#include "RHI/Vulkan/VulkanConstants.h"

#include <unordered_map>

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

        struct DescriptorSetEntry {
            vk::raii::DescriptorSet             set          = nullptr;
            std::array<uint32_t, k_MaxBindings> boundBuffers = {}; // IDs of buffers bound
        };
        std::unordered_map<uint32_t, DescriptorSetEntry> m_Sets; // Keyed by pipeline ID

    public:
        VulkanDescriptorSetAllocator(VulkanContext& context);

        // Returns existing or allocates new descriptor set for this pipeline layout
        vk::DescriptorSet GetOrAllocate(uint32_t pipelineId, vk::DescriptorSetLayout layout);

        // Returns true if the buffer bound to this slot has changed since last write
        bool NeedsWrite(uint32_t pipelineId, uint32_t binding, uint32_t bufferId) const;

        // Records that this buffer is now bound to this slot
        void MarkWritten(uint32_t pipelineId, uint32_t binding, uint32_t bufferId);

        void Reset();
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR
