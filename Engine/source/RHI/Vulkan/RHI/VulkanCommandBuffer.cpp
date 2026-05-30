#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "RHI/Vulkan/VulkanResourceData.h"
#include "RHI/Vulkan/VulkanFrame.h"
#include "Engine/Core/Assert.h"

namespace Engine::RHI::Vulkan
{
        VulkanCommandBuffer::VulkanCommandBuffer(VulkanGraphicsDevice& graphicsDevice, vk::CommandPool commandPool)
            : m_GraphicsDevice(graphicsDevice)
        {
            vk::CommandBufferAllocateInfo allocInfo(
                commandPool,
                vk::CommandBufferLevel::ePrimary,
                1
            );

            m_CommandBuffer = std::move(vk::raii::CommandBuffers(m_GraphicsDevice.GetContext().GetDevice(), allocInfo).front());
        }

        // Graphics
        void VulkanCommandBuffer::BindPipeline(PipelineHandle pipeline)
        {
            // Get data and bind
            VulkanPipelineData& data = m_GraphicsDevice.GetPipelineData(pipeline);
            m_BoundPipelineHandle = pipeline;
            m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, data.pipeline);
        }

        void VulkanCommandBuffer::BindVertexBuffer(BufferHandle buffer)
        {
            // Get data
            VulkanBufferData& bdata = m_GraphicsDevice.GetBufferData(buffer);

            ENGINE_CORE_ASSERT(bdata.desc.type == BufferType::Vertex, "VulkanCommandBuffer: BindVertexBuffer(): buffer is not a vertex buffer!");

            vk::Buffer vkBuffer = nullptr;
            uint32_t dynamicOffset = 0;

            switch(bdata.desc.usage)
            {
                case BufferUsage::Static:
                {
                    vkBuffer = bdata.buffer;
                    break;
                }
                
                case BufferUsage::Dynamic:
                {
                    vkBuffer = m_GraphicsDevice.GetCurrentFrame()->GetVertexDynamicBufferAllocator().GetBuffer();
                    dynamicOffset = bdata.dynamicOffsets[m_GraphicsDevice.GetFrameIndex()];
                    break;
                }
            }

            m_CommandBuffer.bindVertexBuffers(0, {vkBuffer}, {dynamicOffset});
        }

        void VulkanCommandBuffer::BindIndexBuffer(BufferHandle buffer)
        {
            // Get data
            VulkanBufferData& bdata = m_GraphicsDevice.GetBufferData(buffer);

            ENGINE_CORE_ASSERT(bdata.desc.type == BufferType::Index, "VulkanCommandBuffer: BindIndexBuffer(): buffer is not an index buffer!");

            vk::Buffer vkBuffer = nullptr;
            uint32_t dynamicOffset = 0;

            switch(bdata.desc.usage)
            {
                case BufferUsage::Static:
                {
                    vkBuffer = bdata.buffer;
                    break;
                }
                
                case BufferUsage::Dynamic:
                {
                    vkBuffer = m_GraphicsDevice.GetCurrentFrame()->GetIndexDynamicBufferAllocator().GetBuffer();
                    dynamicOffset = bdata.dynamicOffsets[m_GraphicsDevice.GetFrameIndex()];
                    break;
                }
            }

            // TODO: Vulkan: Change index size
            m_CommandBuffer.bindIndexBuffer(vkBuffer, dynamicOffset, vk::IndexType::eUint16);
        }

        void VulkanCommandBuffer::BindUniformBuffer(BufferHandle buffer, uint32_t binding)
        {
            ENGINE_CORE_ASSERT(m_BoundPipelineHandle.IsValid(), "VulkanCommandBuffer: BindUniformBuffer(): no bound pipeline!");
            
            // Get data
            VulkanPipelineData& pdata = m_GraphicsDevice.GetPipelineData(m_BoundPipelineHandle);
            VulkanBufferData& bdata = m_GraphicsDevice.GetBufferData(buffer);

            ENGINE_CORE_ASSERT(bdata.desc.type == BufferType::Uniform, "VulkanCommandBuffer: BindUniformBuffer(): buffer is not a uniform buffer!");

            // Get buffer
            vk::Buffer vkBuffer = nullptr;
            uint32_t dynamicOffset = 0;

            switch(bdata.desc.usage)
            {
                case BufferUsage::Static:
                {
                    vkBuffer = bdata.buffer;
                    break;
                }
                
                case BufferUsage::Dynamic:
                {
                    vkBuffer = m_GraphicsDevice.GetCurrentFrame()->GetUniformDynamicBufferAllocator().GetBuffer();
                    dynamicOffset = bdata.dynamicOffsets[m_GraphicsDevice.GetFrameIndex()];
                    break;
                }
            }

            // Get descriptor set
            VulkanDescriptorSetAllocator& alloc = m_GraphicsDevice.GetCurrentFrame()->GetDescriptorSetAllocator();
            vk::DescriptorSet set = alloc.GetOrAllocate(m_BoundPipelineHandle.id, *pdata.descriptorSetLayout);

            // Check if descriptor set needs written
            if (alloc.NeedsWriteBuffer(m_BoundPipelineHandle.id, binding, buffer.id))
            {
                vk::DescriptorBufferInfo bufferInfo;
                bufferInfo.buffer = vkBuffer;
                bufferInfo.offset = 0;
                bufferInfo.range  = bdata.desc.size;

                vk::WriteDescriptorSet write;
                write.dstSet          = set;
                write.dstBinding      = binding;
                write.descriptorCount = 1;
                write.descriptorType  = vk::DescriptorType::eUniformBufferDynamic;
                write.pBufferInfo     = &bufferInfo;

                m_GraphicsDevice.GetContext().GetDevice().updateDescriptorSets(write, {});

                alloc.MarkWrittenBuffer(m_BoundPipelineHandle.id, binding, buffer.id);
            }

            alloc.SetDynamicOffset(m_BoundPipelineHandle.id, binding, dynamicOffset);
        }

        void VulkanCommandBuffer::BindTexture(TextureHandle texture, uint32_t binding)
        {
            ENGINE_CORE_ASSERT(m_BoundPipelineHandle.IsValid(), "VulkanCommandBuffer: BindUniformBuffer(): no bound pipeline!");

            // Get Data
            VulkanTextureData&  tdata = m_GraphicsDevice.GetTextureData(texture);
            VulkanPipelineData& pdata = m_GraphicsDevice.GetPipelineData(m_BoundPipelineHandle);

            // Get descriptor set
            VulkanDescriptorSetAllocator& alloc = m_GraphicsDevice.GetCurrentFrame()->GetDescriptorSetAllocator();
            vk::DescriptorSet set = alloc.GetOrAllocate(m_BoundPipelineHandle.id, *pdata.descriptorSetLayout);

            // Check if descriptor set needs written
            if (alloc.NeedsWriteTexture(m_BoundPipelineHandle.id, binding, texture.id))
            {
                vk::DescriptorImageInfo imageInfo;
                imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                imageInfo.imageView   = *tdata.imageView;
                imageInfo.sampler     = *tdata.sampler;

                vk::WriteDescriptorSet write;
                write.dstSet          = set;
                write.dstBinding      = binding;
                write.descriptorCount = 1;
                write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
                write.pImageInfo      = &imageInfo;

                m_GraphicsDevice.GetContext().GetDevice().updateDescriptorSets(write, {});

                alloc.MarkWrittenTexture(m_BoundPipelineHandle.id, binding, texture.id);
            }

            alloc.MarkDirty(m_BoundPipelineHandle.id);
        }

        void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
        {
            if(m_BoundPipelineHandle.IsValid())
            {
                VulkanPipelineData& pdata = m_GraphicsDevice.GetPipelineData(m_BoundPipelineHandle);
                m_GraphicsDevice.GetCurrentFrame()->GetDescriptorSetAllocator().BindDescriptorSets(
                    m_BoundPipelineHandle.id,
                    pdata.descriptorSetLayout,
                    pdata.pipelineLayout,
                    m_CommandBuffer
                );
            }
            m_CommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
        }

        void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t indexOffset, uint32_t firstInstance)
        {
            if(m_BoundPipelineHandle.IsValid())
            {
                VulkanPipelineData& pdata = m_GraphicsDevice.GetPipelineData(m_BoundPipelineHandle);
                m_GraphicsDevice.GetCurrentFrame()->GetDescriptorSetAllocator().BindDescriptorSets(
                    m_BoundPipelineHandle.id,
                    pdata.descriptorSetLayout,
                    pdata.pipelineLayout,
                    m_CommandBuffer
                );
            }
            m_CommandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, indexOffset, firstInstance);
        }

   
        // Data
        void VulkanCommandBuffer::UploadBuffer(BufferHandle buffer, void* data, size_t size, size_t offset)
        {
            // Get data
            VulkanBufferData& bdata = m_GraphicsDevice.GetBufferData(buffer);

            switch(bdata.desc.usage)
            {
                case BufferUsage::Static:
                {
                    // Create staging buffer
                    VkBufferCreateInfo stagingBufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
                    stagingBufferInfo.size = size;
                    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

                    // Allocation info + request memory mapping
                    VmaAllocationCreateInfo allocInfo{};
                    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
                    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

                    // Create & allocate staging buffer
                    VmaAllocationInfo resultInfo;
                    VkBuffer stagingBuffer;
                    VmaAllocation stagingAllocation;
                    vmaCreateBuffer(m_GraphicsDevice.GetContext().GetAllocator(), &stagingBufferInfo, &allocInfo, &stagingBuffer, &stagingAllocation, &resultInfo);

                    // Copy data to staging buffer
                    std::memcpy(resultInfo.pMappedData, data, size);

                    // Copy command from staging buffer to static buffer
                    vk::BufferCopy copyRegion{};
                    copyRegion.srcOffset = 0;
                    copyRegion.dstOffset = offset;
                    copyRegion.size = size;
                    m_CommandBuffer.copyBuffer(stagingBuffer, bdata.buffer, { copyRegion });

                    // Add to staging buffer allocations
                    m_StagingBufferAllocations.emplace_back(stagingBuffer, stagingAllocation);
                    break;
                }

                case BufferUsage::Dynamic:
                {
                    VulkanDynamicBufferAllocator* alloc = nullptr;
                    switch(bdata.desc.type)
                    {
                        case BufferType::Vertex:  alloc = &m_GraphicsDevice.GetCurrentFrame()->GetVertexDynamicBufferAllocator(); break;
                        case BufferType::Index:   alloc = &m_GraphicsDevice.GetCurrentFrame()->GetIndexDynamicBufferAllocator(); break;
                        case BufferType::Uniform: alloc = &m_GraphicsDevice.GetCurrentFrame()->GetUniformDynamicBufferAllocator(); break;
                    }

                    bdata.dynamicOffsets[m_GraphicsDevice.GetFrameIndex()] = alloc->AllocateAndCopy(data, size);
                    break;
                }
            }
        }

        void VulkanCommandBuffer::UploadTexture(TextureHandle texture, void* data)
        {
            // Get data
            VulkanTextureData& tdata = m_GraphicsDevice.GetTextureData(texture);
            size_t size = tdata.desc.width * tdata.desc.height * VulkanCommon::GetPixelSize(tdata.desc.format);

            // Create staging buffer
            VkBufferCreateInfo stagingBufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
            stagingBufferInfo.size = size;
            stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            // Allocation info + request memory mapping
            VmaAllocationCreateInfo allocInfo{};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

            // Create & allocate staging buffer
            VmaAllocationInfo resultInfo;
            VkBuffer stagingBuffer;
            VmaAllocation stagingAllocation;
            vmaCreateBuffer(m_GraphicsDevice.GetContext().GetAllocator(), &stagingBufferInfo, &allocInfo, &stagingBuffer, &stagingAllocation, &resultInfo);

            // Copy data to staging buffer
            std::memcpy(resultInfo.pMappedData, data, size);

            // Transition image layout eUndefined->eTransferDstOptimal
            VulkanCommon::TransitionImageLayout(
                m_CommandBuffer,
                tdata.image, 
                vk::ImageLayout::eUndefined, 
                vk::ImageLayout::eTransferDstOptimal,
                vk::AccessFlagBits2::eNone,
                vk::AccessFlagBits2::eTransferWrite,
                vk::PipelineStageFlagBits2::eTopOfPipe,
                vk::PipelineStageFlagBits2::eTransfer,
                vk::ImageAspectFlagBits::eColor
            );

            // Copy command from staging buffer to image
            vk::BufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = vk::Offset3D{0, 0, 0};
            region.imageExtent = vk::Extent3D{ tdata.desc.width, tdata.desc.height, 1 };

            vk::BufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = size;

            m_CommandBuffer.copyBufferToImage(
                stagingBuffer,
                tdata.image,
                vk::ImageLayout::eTransferDstOptimal,
                region
            );

            // eTransferDstOptimal -> eShaderReadOnlyOptimal
            VulkanCommon::TransitionImageLayout(
                m_CommandBuffer,
                tdata.image, 
                vk::ImageLayout::eTransferDstOptimal, 
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::AccessFlagBits2::eTransferWrite,
                vk::AccessFlagBits2::eShaderRead,
                vk::PipelineStageFlagBits2::eTransfer,
                vk::PipelineStageFlagBits2::eFragmentShader,
                vk::ImageAspectFlagBits::eColor
            );

            // Add to staging buffer allocations
            m_StagingBufferAllocations.emplace_back(stagingBuffer, stagingAllocation);
        }

        // Begin/End* for Vulkan classes
        void VulkanCommandBuffer::BeginRendering(VulkanTextureData* renderTarget, Vec4 clearColor, VulkanTextureData* depthBuffer)
        {
            ENGINE_CORE_ASSERT(m_CurrentRenderTarget == nullptr, "Vulkan: VulkanCommandBuffer: BeginRendering(): Already in a render pass!");
            ENGINE_CORE_ASSERT(renderTarget != nullptr, "Vulkan: VulkanCommandBuffer: BeginRendering(): renderTarget is nullptr!");

            m_CurrentRenderTarget = renderTarget;
            uint32_t width = m_CurrentRenderTarget->desc.width;
            uint32_t height = m_CurrentRenderTarget->desc.height;

            // We assume that the texture passed to us has TextureUsage::RenderTarget.

            m_CommandBuffer.begin({});

            // Transition current image for color attachment
            VulkanCommon::TransitionImageLayout(
                *m_CommandBuffer,
                m_CurrentRenderTarget->image,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eColorAttachmentOptimal,
                {},
                vk::AccessFlagBits2::eColorAttachmentWrite,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                vk::ImageAspectFlagBits::eColor
            );

            // Rendering info
            vk::ClearValue clr = vk::ClearColorValue(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            vk::RenderingAttachmentInfo attachmentInfo(
                m_CurrentRenderTarget->imageView,
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
                    {width, height}
                },
                1,
                0,
                1, &attachmentInfo
            );

            // Depth buffer
            if (depthBuffer != nullptr)
            {
                // Transition depth image
                VulkanCommon::TransitionImageLayout(
                    *m_CommandBuffer,
                    depthBuffer->image,
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    {},
                    vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
                    vk::PipelineStageFlagBits2::eEarlyFragmentTests,
                    vk::PipelineStageFlagBits2::eEarlyFragmentTests,
                    vk::ImageAspectFlagBits::eDepth
                );

                vk::ClearValue depthClear;
                depthClear.depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

                vk::RenderingAttachmentInfo depthAttachment(
                    *depthBuffer->imageView,
                    vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    vk::ResolveModeFlagBits::eNone,
                    {},
                    vk::ImageLayout::eUndefined,
                    vk::AttachmentLoadOp::eClear,
                    vk::AttachmentStoreOp::eStore,
                    depthClear
                );

                renderingInfo.pDepthAttachment = &depthAttachment;
            }

            // Begin rendering & viewport
            m_CommandBuffer.beginRendering(renderingInfo);
            m_CommandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f));
            m_CommandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(width, height)));
        }

        void VulkanCommandBuffer::EndRendering()
        {
            ENGINE_CORE_ASSERT(m_CurrentRenderTarget != nullptr, "Vulkan: VulkanCommandBuffer: BeginRendering(): Not in a render pass!");

            m_CommandBuffer.endRendering();

            VulkanCommon::TransitionImageLayout(
                *m_CommandBuffer,
                m_CurrentRenderTarget->image,
                vk::ImageLayout::eColorAttachmentOptimal,
                vk::ImageLayout::ePresentSrcKHR,
                vk::AccessFlagBits2::eColorAttachmentWrite,
                {},
                vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                vk::PipelineStageFlagBits2::eBottomOfPipe,
                vk::ImageAspectFlagBits::eColor
            );

            m_CommandBuffer.end();
            m_CurrentRenderTarget = nullptr;
        }

        void VulkanCommandBuffer::BeginImmediate()
        {
            m_CommandBuffer.begin({});
        }

        void VulkanCommandBuffer::EndImmediate()
        {
            m_CommandBuffer.end();
        }

        // Resetter for Vulkan classes
        void VulkanCommandBuffer::Reset()
        {
            m_CurrentRenderTarget = nullptr;
            m_BoundPipelineHandle = PipelineHandle{.id = 0};
        
            // Clear staging buffer allocations
            for(StagingBufferAllocation& alloc : m_StagingBufferAllocations)
            {
                vmaDestroyBuffer(m_GraphicsDevice.GetContext().GetAllocator(), alloc.buffer, alloc.allocation);
            }
            m_StagingBufferAllocations.clear();
        }

} // namespace Engine::RHI::Vulkan
