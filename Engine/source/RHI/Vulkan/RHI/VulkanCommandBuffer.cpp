#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "RHI/Vulkan/RHI/VulkanTexture.h"
#include "RHI/Vulkan/RHI/VulkanPipeline.h"

#include "Engine/Core/Assert.h"

#include "RHI/Vulkan/VulkanCommon.h"

namespace Engine
{
    VulkanCommandBuffer::VulkanCommandBuffer(VulkanContext* context, vk::CommandPool pool)
    {
        ENGINE_CORE_ASSERT(context != nullptr, "Vulkan: context is nullptr!");

        // Create command buffer
        vk::CommandBufferAllocateInfo allocInfo(
            pool,
            vk::CommandBufferLevel::ePrimary,
            1
        );

        m_CommandBuffer = std::move(vk::raii::CommandBuffers(context->GetDevice(), allocInfo).front());
    }


    void VulkanCommandBuffer::Begin()
    {
        m_CommandBuffer.begin({});
    }

    void VulkanCommandBuffer::End()
    {
        m_CommandBuffer.end();
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
        m_CurrentRenderTarget = static_cast<VulkanTexture*>(renderTarget);

        // Transition image layout for color attatchment
        VulkanCommon::TransitionImageLayout(
            m_CommandBuffer,
            m_CurrentRenderTarget->GetImage(),
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput
        );

        vk::ClearValue clr = vk::ClearColorValue(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        vk::RenderingAttachmentInfo attachmentInfo(
            m_CurrentRenderTarget->GetImageView(),
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
                {m_CurrentRenderTarget->GetWidth(), m_CurrentRenderTarget->GetHeight()}
            },
            1,
            0,
            1, &attachmentInfo
        );

        m_CommandBuffer.beginRendering(renderingInfo);

        m_CommandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_CurrentRenderTarget->GetWidth()), static_cast<float>(m_CurrentRenderTarget->GetHeight()), 0.0f, 1.0f));
        m_CommandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(m_CurrentRenderTarget->GetWidth(), m_CurrentRenderTarget->GetHeight())));
    }

    void VulkanCommandBuffer::EndRendering()
    {
        m_CommandBuffer.endRendering();

        VulkanCommon::TransitionImageLayout(
            m_CommandBuffer,
            m_CurrentRenderTarget->GetImage(),
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            {},
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eBottomOfPipe
        );

        m_CurrentRenderTarget = nullptr;
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

    void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        m_CommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }


} // namespace Engine
