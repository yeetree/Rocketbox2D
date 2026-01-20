#include "Renderer/Vulkan/VulkanBuffer.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    VulkanBuffer::VulkanBuffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const BufferDesc& desc) {
        // Create buffer
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.size = desc.size;
        bufferInfo.usage = GetVulkanBufferUsage(desc.type);
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;
        m_Buffer = vk::raii::Buffer(device, bufferInfo);

        // Get memory requirements and properties
        vk::MemoryRequirements memRequirements = m_Buffer.getMemoryRequirements();
        vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

        // Allocate memory for buffer
        vk::MemoryAllocateInfo memoryAllocateInfo;
        memoryAllocateInfo.allocationSize = memRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, memProperties, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        m_BufferMemory = vk::raii::DeviceMemory( device, memoryAllocateInfo );

        // Bind memory to buffer
        m_Buffer.bindMemory( *m_BufferMemory, 0 );

        // Set data
        if(desc.data) {
            void* data = m_BufferMemory.mapMemory(0, bufferInfo.size);
            memcpy(data, desc.data, bufferInfo.size);
            m_BufferMemory.unmapMemory();
        }
    }

    VulkanBuffer::~VulkanBuffer() {
        
    }

    vk::BufferUsageFlagBits VulkanBuffer::GetVulkanBufferUsage(BufferType type) {
        switch(type) {
            case BufferType::Vertex:    return vk::BufferUsageFlagBits::eVertexBuffer; break;
            case BufferType::Index:     return vk::BufferUsageFlagBits::eIndexBuffer; break;
            case BufferType::Uniform:   return vk::BufferUsageFlagBits::eUniformBuffer; break;
        }
        return vk::BufferUsageFlagBits::eVertexBuffer; // Shouldn't really happen
    }

    uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, vk::PhysicalDeviceMemoryProperties memProperties, vk::MemoryPropertyFlags properties) {
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        LOG_CORE_ERROR("Vulkan: Failed to find suitable memory type when creating buffer!");
        return 0; // If stuff starts crashing, this is why
    }
} // namespace Engine
