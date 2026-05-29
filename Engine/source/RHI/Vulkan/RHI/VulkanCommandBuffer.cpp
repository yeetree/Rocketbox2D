#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "RHI/Vulkan/RHI/VulkanTexture.h"
#include "RHI/Vulkan/RHI/VulkanPipeline.h"
#include "RHI/Vulkan/RHI/VulkanBuffer.h"

#include "Engine/Core/Assert.h"

#include "RHI/Vulkan/VulkanCommon.h"

namespace Engine
{
    VulkanCommandBuffer::VulkanCommandBuffer(VulkanContext* context, vk::CommandPool pool)
        : m_Context(context)
    {
        ENGINE_CORE_ASSERT(context != nullptr, "Vulkan: context is nullptr!");

        // Create command buffer
        vk::CommandBufferAllocateInfo allocInfo(
            pool,
            vk::CommandBufferLevel::ePrimary,
            1
        );

        m_CommandBuffer = std::move(vk::raii::CommandBuffers(context->GetDevice(), allocInfo).front());
    
        m_Allocator = context->GetAllocator();
    }

    vk::Buffer VulkanCommandBuffer::GetVulkanBuffer(VulkanBuffer* buffer)
    {
        vk::Buffer vkbuf;

        switch(buffer->GetUsage())
        {
            case BufferUsage::Static: vkbuf = buffer->GetStaticBuffer(); break;
            case BufferUsage::Dynamic:
            {
                if(m_FrameIndex == -1 || m_Frame == nullptr)
                {
                    LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: GetVulkanBuffer(): cannot bind dynamic buffer without frame info!");
                    return nullptr;
                }
                vkbuf = m_Frame->GetDynamicBuffer(buffer->GetType())->GetBuffer();
            }
        }
        
        return vkbuf;
    }


    void VulkanCommandBuffer::Begin()
    {
        m_CommandBuffer.begin({});
    }

    void VulkanCommandBuffer::End()
    {
        m_CommandBuffer.end();

        // Clean info
        m_FrameIndex = -1;
        m_Frame = nullptr;
    }

    // Render target
    void VulkanCommandBuffer::BeginRendering(ITexture* renderTarget, Vec4 clearColor)
    {
        // Verify render target
        if(renderTarget == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: BeginRendering(): renderTarget is nullptr!");
            return;
        }

        if(renderTarget->GetUsage() & TextureUsage::RenderTarget == 0)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: BeginRendering(): renderTarget is not RenderTarget!");
            return;
        }

        // Get vulkan texture
        VulkanTexture* vt = static_cast<VulkanTexture*>(renderTarget);

        // Transition image layout for color attatchment
        VulkanCommon::TransitionImageLayout(
            m_CommandBuffer,
            vt->GetImage(),
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput
        );

        vk::ClearValue clr = vk::ClearColorValue(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        vk::RenderingAttachmentInfo attachmentInfo(
            vt->GetImageView(),
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ResolveModeFlagBits::eNone,
            {},
            vk::ImageLayout::eUndefined,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            clr
        );

        vk::RenderingInfo renderingInfo(
            {},
            {
                {0, 0},
                {vt->GetWidth(), vt->GetHeight()}
            },
            1,
            0,
            1, &attachmentInfo
        );

        m_CommandBuffer.beginRendering(renderingInfo);

        m_CommandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(vt->GetWidth()), static_cast<float>(vt->GetHeight()), 0.0f, 1.0f));
        m_CommandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(vt->GetWidth(), vt->GetHeight())));
    }

    void VulkanCommandBuffer::EndRendering(ITexture* renderTarget)
    {
        // Verify render target
        if(renderTarget == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: EndRendering(): renderTarget is nullptr!");
            return;
        }

        if(renderTarget->GetUsage() & TextureUsage::RenderTarget == 0)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: EndRendering(): renderTarget is not RenderTarget!");
            return;
        }

        // Get vulkan texture
        VulkanTexture* vt = static_cast<VulkanTexture*>(renderTarget);

        m_CommandBuffer.endRendering();

        VulkanCommon::TransitionImageLayout(
            m_CommandBuffer,
            vt->GetImage(),
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            {},
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eBottomOfPipe
        );
    }

    void VulkanCommandBuffer::BindPipeline(IPipeline* pipeline)
    {
        if(pipeline == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: BindPipeline(): pipeline is nullptr!");
            return;
        }

        VulkanPipeline* vpipe = static_cast<VulkanPipeline*>(pipeline);

        m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vpipe->GetPipeline());
    }

    void VulkanCommandBuffer::BindVertexBuffer(IBuffer* buffer) 
    {
        if(buffer == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: BindVertexBuffer(): buffer is nullptr!");
            return;
        }

        VulkanBuffer* vb = static_cast<VulkanBuffer*>(buffer);

        vk::Buffer vkbuf = GetVulkanBuffer(vb);
        if(vkbuf == nullptr)
        {
            return;
        }

        size_t offset = vb->GetOffset(m_FrameIndex);

        m_CommandBuffer.bindVertexBuffers(0, {vkbuf}, {offset});
    }

    void VulkanCommandBuffer::BindIndexBuffer(IBuffer* buffer)
    {
        if(buffer == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: BindVertexBuffer(): buffer is nullptr!");
            return;
        }

        VulkanBuffer* vb = static_cast<VulkanBuffer*>(buffer);

        vk::Buffer vkbuf = GetVulkanBuffer(vb);
        if(vkbuf == nullptr)
        {
            return;
        }

        size_t offset = vb->GetOffset(m_FrameIndex);

        // TODO: Vulkan: Change index size
        m_CommandBuffer.bindIndexBuffer(vkbuf, offset, vk::IndexType::eUint16);
    }

    void VulkanCommandBuffer::BindUniformBuffer(IBuffer* buffer, IPipeline* pipeline, uint32_t binding)
    {
        if(m_FrameIndex == -1 || m_Frame == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: BindUniformBuffer(): cannot bind uniform buffer without frame info!");
            return;
        }

        if(buffer == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: BindUniformBuffer(): buffer is nullptr!");
            return;
        }

        if(pipeline == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: BindUniformBuffer(): pipeline is nullptr!");
            return;
        }

        VulkanBuffer* vb = static_cast<VulkanBuffer*>(buffer);
        VulkanPipeline* vp = static_cast<VulkanPipeline*>(pipeline);

        // Get set
        vk::DescriptorSet set = m_Frame->GetDescriptorSetAllocator()->GetOrAllocate(vp);

        // Write it
        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = m_Frame->GetDynamicBuffer(vb->GetType())->GetBuffer();
        bufferInfo.offset = 0; // Base
        bufferInfo.range  = vb->GetSize();

        vk::WriteDescriptorSet write;
        write.dstSet = set;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = vk::DescriptorType::eUniformBufferDynamic;
        write.pBufferInfo = &bufferInfo;

        m_CommandBuffer.getDevice().updateDescriptorSets(write, {});

        uint32_t offset = static_cast<uint32_t>(vb->GetOffset(m_FrameIndex));

        // Bind
        m_CommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            vp->GetPipelineLayout(),
            0, { set },
            { offset } 
        );
    }

    void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        m_CommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t indexOffset, uint32_t firstInstance)
    {
        m_CommandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, indexOffset, firstInstance);
    }

    void VulkanCommandBuffer::SetBufferData(IBuffer* buffer, void* data, size_t size, size_t offset)
    {
        if(buffer == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: SetBufferData(): buffer is nullptr!");
            return;
        }

        VulkanBuffer* vb = static_cast<VulkanBuffer*>(buffer);
        
        if(vb->GetUsage() != BufferUsage::Static)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: SetBufferData(): buffer is not static!");
            return;
        }

        // Create staging buffer
        VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo resultInfo;
        VkBuffer stagingBuffer;
        VmaAllocation stagingAllocation;
        
        vmaCreateBuffer(m_Allocator, &bufferInfo, &allocInfo, &stagingBuffer, &stagingAllocation, &resultInfo);

        // Copy data
        std::memcpy(resultInfo.pMappedData, data, size);

        // Copy
        vk::BufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = offset;
        copyRegion.size = size;

        m_CommandBuffer.copyBuffer(stagingBuffer, vb->GetStaticBuffer(), { copyRegion });

        m_StagingBufferAllocations.emplace_back(stagingBuffer, stagingAllocation);
    }

    void VulkanCommandBuffer::SetTextureData(ITexture* texture, void* data)
    {
        if(texture == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanCommandBuffer: SetTextureData(): texture is nullptr!");
            return;
        }

        VulkanTexture* vt = static_cast<VulkanTexture*>(texture);

        // Create the staging buffer
        BufferDesc stagingDesc;
        stagingDesc.size = vt->GetWidth() * vt->GetHeight() * 4; // RGBA8
        stagingDesc.type = BufferType::Vertex;  // Generic memory type
        stagingDesc.usage == BufferUsage::Static;
        VulkanBuffer stagingBuffer(m_Context, stagingDesc);

        // Upload data
        SetBufferData(&stagingBuffer, data, stagingBuffer.GetSize(), 0);

        // Copy

        // undefined -> transferdstoptimal
        VulkanCommon::TransitionImageLayout(
            m_CommandBuffer,
            vt->GetImage(), 
            vk::ImageLayout::eUndefined, 
            vk::ImageLayout::eTransferDstOptimal,
            vk::AccessFlagBits2::eNone,                   // src access
            vk::AccessFlagBits2::eTransferWrite,          // dst access
            vk::PipelineStageFlagBits2::eTopOfPipe,       // src stage
            vk::PipelineStageFlagBits2::eTransfer         // dst stage
        );

        // copy buffer to image
        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = vk::Offset3D{0, 0, 0};
        region.imageExtent = vk::Extent3D{ vt->GetWidth(), vt->GetHeight(), 1 };

        m_CommandBuffer.copyBufferToImage(
            static_cast<vk::Buffer>(stagingBuffer.GetStaticBuffer()),
            vt->GetImage(),
            vk::ImageLayout::eTransferDstOptimal,
            region
        );

        // transferdstoptimal -> shaderread
        VulkanCommon::TransitionImageLayout(
            m_CommandBuffer,
            vt->GetImage(), 
            vk::ImageLayout::eTransferDstOptimal, 
            vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::AccessFlagBits2::eTransferWrite,          // src access
            vk::AccessFlagBits2::eShaderRead,             // dst access
            vk::PipelineStageFlagBits2::eTransfer,        // src stage
            vk::PipelineStageFlagBits2::eFragmentShader   // dst stage
        );
    }

    void VulkanCommandBuffer::SetFrameInfo(uint32_t frameIdx, VulkanFrame* frame)
    {
        m_FrameIndex = frameIdx;
        m_Frame = frame;
    }

    void VulkanCommandBuffer::FreeStagingBufferAllocations()
    {
        for (auto& staging : m_StagingBufferAllocations)
        {
            vmaDestroyBuffer(m_Allocator, staging.buffer, staging.allocation);
        }
        m_StagingBufferAllocations.clear();
    }

} // namespace Engine
