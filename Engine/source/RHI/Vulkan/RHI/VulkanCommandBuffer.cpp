#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "RHI/Vulkan/VulkanResourceData.h"
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
            m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, data.pipeline);
        }

        void VulkanCommandBuffer::BindVertexBuffer(BufferHandle buffer)
        {

        }

        void VulkanCommandBuffer::BindIndexBuffer(BufferHandle buffer)
        {

        }

        void VulkanCommandBuffer::BindUniformBuffer(BufferHandle buffer, uint32_t binding)
        {

        }

        void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
        {
            m_CommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
        }

        void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t indexOffset, uint32_t firstInstance)
        {
            m_CommandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, indexOffset, firstInstance);
        }

   
        // Data
        void VulkanCommandBuffer::UploadBuffer(BufferHandle buffer, void* data, size_t size, size_t offset)
        {

        }

        void VulkanCommandBuffer::UploadTexture(TextureHandle texture, void* data)
        {

        }

        // Begin/EndRendering for Vulkan classes
        void VulkanCommandBuffer::BeginRendering(VulkanTextureData* renderTarget, Vec4 clearColor)
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
                vk::PipelineStageFlagBits2::eColorAttachmentOutput
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
                vk::PipelineStageFlagBits2::eBottomOfPipe
            );

            m_CommandBuffer.end();
            m_CurrentRenderTarget = nullptr;
        }

        // Resetter for Vulkan classes
        void VulkanCommandBuffer::Reset()
        {
            m_CurrentRenderTarget = nullptr;
        }

} // namespace Engine::RHI::Vulkan
