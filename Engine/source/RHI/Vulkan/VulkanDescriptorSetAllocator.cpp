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

    bool VulkanDescriptorSetAllocator::NeedsWrite(uint32_t pipelineId, uint32_t binding, uint32_t bufferId) const
    {
        // Check cache: If not allocated, needs write
        auto it = m_Sets.find(pipelineId);
        if (it == m_Sets.end())
            return true;

        // Else, check if this binding is this buffer
        return it->second.boundBuffers[binding] != bufferId;
    }

    void VulkanDescriptorSetAllocator::MarkWritten(uint32_t pipelineId, uint32_t binding, uint32_t bufferId)
    {
        auto it = m_Sets.find(pipelineId);
        if (it != m_Sets.end())
            it->second.boundBuffers[binding] = bufferId;
    }

    void VulkanDescriptorSetAllocator::Reset()
    {
        //m_Sets.clear();
    }
} // namespace Engine::RHI::Vulkan
