#include "Renderer/Vulkan/RHI/VulkanUniformBuffer.h"
#include "Renderer/Vulkan/RHI/VulkanBuffer.h"
#include "Renderer/Vulkan/VulkanConstants.h"
#include "Renderer/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanUniformBuffer::VulkanUniformBuffer(VulkanGraphicsDevice* graphicsDevice, const UniformBufferDesc& desc) 
        : m_GraphicsDevice(graphicsDevice) 
    {
        ENGINE_CORE_ASSERT(graphicsDevice != nullptr, "Vulkan: invalid graphics device when creating uniform buffer!");

        // Create internal VulkanBuffers
        m_Buffers.reserve(k_MaxFramesInFlight);
        BufferDesc internalDesc;
        internalDesc.size = desc.size;
        internalDesc.type = BufferType::Uniform;
        internalDesc.isDynamic = true;

        for (uint32_t i = 0; i < k_MaxFramesInFlight; ++i) {
            m_Buffers.emplace_back(CreateScope<VulkanBuffer>(m_GraphicsDevice, internalDesc));
        }

        // Allocate descriptor sets
        std::vector<vk::DescriptorSetLayout> layouts(k_MaxFramesInFlight, m_GraphicsDevice->GetUBODescriptorSetLayout());
        
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = *m_GraphicsDevice->GetDescriptorPool();
        allocInfo.descriptorSetCount = k_MaxFramesInFlight;
        allocInfo.pSetLayouts = layouts.data();

        m_DescriptorSets = m_GraphicsDevice->GetDevice().GetDevice().allocateDescriptorSets(allocInfo);

        // Update descriptor sets
        for (uint32_t i = 0; i < k_MaxFramesInFlight; ++i) {
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_Buffers[i]->GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range  = desc.size;

            vk::WriteDescriptorSet descriptorWrite{};
            descriptorWrite.dstSet          = *m_DescriptorSets[i];
            descriptorWrite.dstBinding      = 0; 
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType  = vk::DescriptorType::eUniformBuffer;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo     = &bufferInfo;

            m_GraphicsDevice->GetDevice().GetDevice().updateDescriptorSets(descriptorWrite, nullptr);
        }

        // Set data
        if (desc.data != nullptr) {
            for (uint32_t i = 0; i < k_MaxFramesInFlight; ++i) {
                m_Buffers[i]->UpdateData(desc.data, desc.size, 0);
            }
        }
    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {

    }

    void VulkanUniformBuffer::UpdateData(const void* data, size_t size, size_t offset) {
        if(data == nullptr) {
            return;
        }

        uint32_t currentFrame = m_GraphicsDevice->GetFrameIndex();
        m_Buffers[currentFrame]->UpdateData(data, size, offset);
    }

    VulkanBuffer& VulkanUniformBuffer::GetBuffer(uint32_t index) {
        return *m_Buffers[index];
    }

    vk::DescriptorSet VulkanUniformBuffer::GetDescriptorSet(uint32_t index) {
        return m_DescriptorSets[index];
    }
} // namespace Engine
