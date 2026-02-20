#include "Renderer/Vulkan/RHI/VulkanUniformBuffer.h"
#include "Renderer/Vulkan/RHI/VulkanBuffer.h"
#include "Renderer/Vulkan/VulkanConstants.h"
#include "Renderer/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanUniformBuffer::VulkanUniformBuffer(VulkanGraphicsDevice* graphicsDevice, const UniformBufferDesc& desc)
        : m_Size(desc.size), m_GraphicsDevice(graphicsDevice) 
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

        // Set data
        if (desc.data != nullptr) {
            for (uint32_t i = 0; i < k_MaxFramesInFlight; ++i) {
                m_Buffers[i]->UpdateData(desc.data, desc.size, 0);
            }
        }
    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {

    }

    size_t VulkanUniformBuffer::GetSize() const {
        return m_Size;
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
} // namespace Engine
