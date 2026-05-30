#include "RHI/Vulkan/VulkanDescriptorSetAllocator.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanConstants.h"

namespace Engine::RHI::Vulkan
{
    VulkanDescriptorSetAllocator::VulkanDescriptorSetAllocator(VulkanContext& context)
        : m_Context(context)
    {
        // Descriptor pool
        std::vector<vk::DescriptorPoolSize> poolSizes = {
            { vk::DescriptorType::eUniformBufferDynamic, k_MaxUniformBuffersPerFrame },
            { vk::DescriptorType::eCombinedImageSampler, k_MaxSamplersPerFrame },
        };

        vk::DescriptorPoolCreateInfo poolInfo;
        poolInfo.flags = {vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet};
        poolInfo.maxSets = k_MaxDescriptorSetsPerFrame;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        m_DescriptorPool = vk::raii::DescriptorPool(m_Context.GetDevice(), poolInfo);
    }

    
    vk::DescriptorSet VulkanDescriptorSetAllocator::GetOrAllocate(uint32_t pipelineId, vk::DescriptorSetLayout layout)
    {
        // Check cache
        auto it = m_Sets.find(pipelineId);
        if (it != m_Sets.end())
            return *it->second.set;

        // Allocate new set
        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.descriptorPool = *m_DescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        DescriptorSetEntry& entry = m_Sets[pipelineId];
        entry.set = std::move(m_Context.GetDevice().allocateDescriptorSets(allocInfo).front());
        entry.boundBuffers.fill(0); // invalid / no buffer

        return *entry.set;
    }

    void VulkanDescriptorSetAllocator::SetDynamicOffset(uint32_t pipelineId, uint32_t binding, uint32_t offset)
    {
        // Get entry
        auto it = m_Sets.find(pipelineId);
        if (it == m_Sets.end()) return;
        DescriptorSetEntry& entry = it->second;
        if (binding >= entry.pendingOffsets.size())
            entry.pendingOffsets.resize(binding + 1, 0);

        if(entry.pendingOffsets[binding] != offset)
        {
            entry.pendingOffsets[binding] = offset;
            entry.dirty = true;
        }
    }

    void VulkanDescriptorSetAllocator::MarkDirty(uint32_t pipelineId)
    {
        auto it = m_Sets.find(pipelineId);
        if (it != m_Sets.end())
            it->second.dirty = true;
    }

    bool VulkanDescriptorSetAllocator::NeedsWriteBuffer(uint32_t pipelineId, uint32_t binding, uint32_t bufferId) const
    {
        // Check cache: If not allocated, needs write
        auto it = m_Sets.find(pipelineId);
        if (it == m_Sets.end())
            return true;

        // Else, check if this binding is this buffer
        return it->second.boundBuffers[binding] != bufferId;
    }

    bool VulkanDescriptorSetAllocator::NeedsWriteTexture(uint32_t pipelineId, uint32_t binding, uint32_t textureId) const
    {
        // Check cache: If not allocated, needs write
        auto it = m_Sets.find(pipelineId);
        if (it == m_Sets.end())
            return true;

        // Else, check if this binding is this texture
        return it->second.boundTextures[binding] != textureId;
    }

    void VulkanDescriptorSetAllocator::MarkWrittenBuffer(uint32_t pipelineId, uint32_t binding, uint32_t bufferId)
    {
        auto it = m_Sets.find(pipelineId);
        if (it != m_Sets.end())
            it->second.boundBuffers[binding] = bufferId;
    }

    void VulkanDescriptorSetAllocator::MarkWrittenTexture(uint32_t pipelineId, uint32_t binding, uint32_t textureId)
    {
        auto it = m_Sets.find(pipelineId);
        if (it != m_Sets.end())
            it->second.boundTextures[binding] = textureId;
    }

    void VulkanDescriptorSetAllocator::BindDescriptorSets(uint32_t pipelineId, vk::DescriptorSetLayout layout, vk::PipelineLayout pipelineLayout, vk::CommandBuffer cmd)
    {
        DescriptorSetEntry& entry = m_Sets.at(pipelineId);
        if (!entry.dirty) return;

        cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipelineLayout,
            0, { *entry.set },
            entry.pendingOffsets
        );

        entry.dirty = false;
        entry.pendingOffsets.clear();
    }

    void VulkanDescriptorSetAllocator::Reset()
    {
        //m_Sets.clear();
    }
} // namespace Engine::RHI::Vulkan
