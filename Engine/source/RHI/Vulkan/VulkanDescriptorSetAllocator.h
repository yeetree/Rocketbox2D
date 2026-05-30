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
            vk::raii::DescriptorSet             set            = nullptr;
            std::array<uint32_t, k_MaxBindings> boundBuffers   = {}; // IDs of buffers bound
            std::array<uint32_t, k_MaxBindings> boundTextures  = {}; // IDs of textures bound
            std::vector<uint32_t>               pendingOffsets = {}; // dynamic offsets
            bool                                dirty          = false;
        };
        std::unordered_map<uint32_t, DescriptorSetEntry> m_Sets; // Keyed by pipeline ID

    public:
        VulkanDescriptorSetAllocator(VulkanContext& context);

        // Returns existing or allocates new descriptor set for this pipeline layout
        vk::DescriptorSet GetOrAllocate(uint32_t pipelineId, vk::DescriptorSetLayout layout);

        // Sets offsets
        void SetDynamicOffset(uint32_t pipelineId, uint32_t binding, uint32_t offset);

        // Mark set dirty for texture bindings
        void MarkDirty(uint32_t pipelineId);

        // Returns true if the resource bound to this slot has changed since last write
        bool NeedsWriteBuffer(uint32_t pipelineId, uint32_t binding, uint32_t bufferId) const;
        bool NeedsWriteTexture(uint32_t pipelineId, uint32_t binding, uint32_t textureId) const;

        // Records that this resource is now bound to this slot
        void MarkWrittenBuffer(uint32_t pipelineId, uint32_t binding, uint32_t bufferId);
        void MarkWrittenTexture(uint32_t pipelineId, uint32_t binding, uint32_t textureId);

        // Binds descriptor sets
        void BindDescriptorSets(uint32_t pipelineId, vk::DescriptorSetLayout layout, vk::PipelineLayout pipelineLayout, vk::CommandBuffer cmd);

        void Reset();
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANDESCRIPTORSETALLOCATOR
