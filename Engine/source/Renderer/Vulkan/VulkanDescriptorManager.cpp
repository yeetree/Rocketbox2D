#include "Renderer/Vulkan/VulkanDescriptorManager.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanConstants.h"

VulkanDescriptorManager::VulkanDescriptorManager(VulkanDevice& device) : m_Device(device) {
    // Create descriptor pool: 1000 uniforms and 1000 textures
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        { vk::DescriptorType::eUniformBuffer, k_MaxUniformBuffersPerFrame },
        { vk::DescriptorType::eCombinedImageSampler, k_MaxSamplersPerFrame }
    };

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    poolInfo.maxSets = 2000; 
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();

    m_Pool = vk::raii::DescriptorPool(m_Device.GetDevice(), poolInfo);
}

vk::DescriptorSet VulkanDescriptorManager::GetDescriptorSet(vk::DescriptorSetLayout layout, const DescriptorSetKey& key) {
    // Get from cache
    auto it = m_SetCache.find(key);
    if (it != m_SetCache.end()) {
        return it->second;
    }

    // Create new descriptor set
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = *m_Pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;
    vk::DescriptorSet ds = (*m_Device.GetDevice()).allocateDescriptorSets(allocInfo)[0];

    // Update set
    std::vector<vk::WriteDescriptorSet> writes;

    std::vector<vk::DescriptorBufferInfo> bufferInfos;
    std::vector<vk::DescriptorImageInfo> imageInfos;
    bufferInfos.reserve(key.buffers.size());
    imageInfos.reserve(key.textures.size());

    // Uniforms
    for (auto const& [binding, info] : key.buffers) {
        bufferInfos.push_back(info);
        vk::WriteDescriptorSet write{};
        write.dstSet = ds;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = vk::DescriptorType::eUniformBuffer;
        write.pBufferInfo = &bufferInfos.back();
        writes.push_back(write);
    }

    // Samplers
    for (auto const& [binding, info] : key.textures) {
        imageInfos.push_back(info);
        vk::WriteDescriptorSet write{};
        write.dstSet = ds;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        write.pImageInfo = &imageInfos.back();
        writes.push_back(write);
    }

    m_Device.GetDevice().updateDescriptorSets(writes, nullptr);

    // Store
    m_SetCache[key] = ds;
    return ds;
}

void VulkanDescriptorManager::Reset() {
    m_SetCache.clear();
    m_Pool.reset();
}