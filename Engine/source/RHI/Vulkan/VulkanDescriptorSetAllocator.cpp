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

    /*
    vk::DescriptorSet VulkanDescriptorSetAllocator::GetOrAllocate(VulkanPipeline* pipeline)
    {
        uint32_t id = pipeline->GetID();

        auto it = m_Sets.find(id);
        if(it != m_Sets.end())
            return it->second;

        vk::DescriptorSetLayout layout = *pipeline->GetDescriptorSetLayout();

        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        auto sets = m_Context->GetDevice().allocateDescriptorSets(allocInfo);
        auto result = m_Sets.emplace(id, std::move(sets.front()));
        return result.first->second;
    }*/

    void VulkanDescriptorSetAllocator::Reset()
    {
        //m_Sets.clear();
    }
} // namespace Engine::RHI::Vulkan
