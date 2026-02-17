#include "Renderer/Vulkan/VulkanGraphicsDevice.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/FileSystem.h"
#include "Renderer/Vulkan/VulkanPipelineState.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanBuffer.h"
#include <SDL3/SDL_vulkan.h>

namespace Engine {

    VulkanGraphicsDevice::VulkanGraphicsDevice(SDL_Window* window) : m_FrameIndex(0), m_ImageIndex(0) {
        LOG_CORE_INFO("Vulkan: Creating Vulkan graphics device...");
        
        m_Context = CreateScope<VulkanContext>(window);
        m_Device = CreateScope<VulkanDevice>(*m_Context);
        m_Swapchain = CreateScope<VulkanSwapchain>(*m_Context, *m_Device);
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice() {
        // Destructor destroys m_Swapchain before m_Device, so we wait
        // for GPU to be done before proceeding with destruction.
        m_Device->GetQueue().waitIdle();

        // Actually we kill m_Swapchain NOW
        m_Swapchain.reset();
    };

    // Utility

    [[nodiscard]] vk::raii::ShaderModule VulkanGraphicsDevice::CreateShaderModule(const std::vector<char>& code) const {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.codeSize = code.size() * sizeof(char);
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        vk::raii::ShaderModule shaderModule{ m_Device->GetDevice(), createInfo };
        return shaderModule;
    }

    void VulkanGraphicsDevice::TransitionImageLayout(
        vk::raii::CommandBuffer& cmd,
        uint32_t imageIndex,
	    vk::ImageLayout         old_layout,
	    vk::ImageLayout         new_layout,
	    vk::AccessFlags2        src_access_mask,
	    vk::AccessFlags2        dst_access_mask,
	    vk::PipelineStageFlags2 src_stage_mask,
	    vk::PipelineStageFlags2 dst_stage_mask)
	{
		vk::ImageMemoryBarrier2 barrier;
        barrier.srcStageMask        = src_stage_mask;
	    barrier.srcAccessMask       = src_access_mask;
		barrier.dstStageMask        = dst_stage_mask;
		barrier.dstAccessMask       = dst_access_mask;
		barrier.oldLayout           = old_layout;
		barrier.newLayout           = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image               = m_Swapchain->GetImage(imageIndex);
		barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel   = 0;
		barrier.subresourceRange.levelCount     = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount     = 1;
		vk::DependencyInfo dependency_info;
		dependency_info.dependencyFlags         = {};
		dependency_info.imageMemoryBarrierCount = 1;
		dependency_info.pImageMemoryBarriers    = &barrier;
		cmd.pipelineBarrier2(dependency_info);
	}

    // Resource Creation
    Scope<IBuffer> VulkanGraphicsDevice::CreateBuffer(const BufferDesc& desc) {
        return CreateScope<VulkanBuffer>(this, desc);
    }

    Scope<ITexture> VulkanGraphicsDevice::CreateTexture(const TextureDesc& desc) {
        return nullptr;
    }

    Scope<IShader> VulkanGraphicsDevice::CreateShader(const ShaderDesc& desc) {
        return CreateScope<VulkanShader>(this, desc);
    }

    Scope<IVertexArray> VulkanGraphicsDevice::CreateVertexArray(const VertexArrayDesc& desc) {
        return nullptr;
    }

    Scope<IPipelineState> VulkanGraphicsDevice::CreatePipelineState(const PipelineDesc& desc) {
        return CreateScope<VulkanPipelineState>(this, desc);
    }

    void VulkanGraphicsDevice::StageBufferUploadData(VulkanBuffer* dstBuffer, const void* data, size_t size, size_t dstOffset) {
        // Create a staging buffer
        BufferDesc stagingDesc;
        stagingDesc.size = size;
        stagingDesc.type = BufferType::Vertex;
        stagingDesc.isDynamic = true;
        stagingDesc.data = data; 
        VulkanBuffer stagingBuffer(this, stagingDesc);

        // Setup the Command Buffer
        vk::CommandBufferAllocateInfo allocInfo(*m_Device->GetCommandPool(), vk::CommandBufferLevel::ePrimary, 1);
        vk::raii::CommandBuffers cmds(m_Device->GetDevice(), allocInfo);
        vk::raii::CommandBuffer& cmd = cmds[0];

        cmd.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

        // Buffer Copy Region
        vk::BufferCopy copyRegion(0, dstOffset, size);
        
        cmd.copyBuffer(*stagingBuffer.m_Buffer, *dstBuffer->m_Buffer, copyRegion);
        cmd.end();

        // Submit and Wait
        vk::SubmitInfo submit;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &*cmd;
        m_Device->GetQueue().submit(submit);
        m_Device->GetQueue().waitIdle();
    }

    // Frame Management
    void VulkanGraphicsDevice::BeginFrame() {    
        // Get frame
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& device = m_Device->GetDevice(); 

        // Wait to finish
        (void)device.waitForFences({*frame.GetInFlightFence()}, vk::True, UINT64_MAX);

        // Get semaphore
        auto& imageAvailableSem = m_Swapchain->GetImageAvailableSemaphore(m_FrameIndex);

        // Get image index to draw on
        auto [result, imageIndex] = m_Swapchain->AcquireNextImage(imageAvailableSem);

        // Resize logic
        if (result == vk::Result::eErrorOutOfDateKHR) {
            int w, h;
            SDL_GetWindowSize(m_Context->GetWindow(), &w, &h);
            Resize(w, h);
            return;
        }

        // Reset fence
        device.resetFences({*frame.GetInFlightFence()});
        m_ImageIndex = imageIndex; // Destination image

        // Start recording commands
        auto& cmd = frame.GetCommandBuffer();
        cmd.reset();
        cmd.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

        TransitionImageLayout(
            cmd,
            m_ImageIndex,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput
        );

        // Set up the color attachment
        vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
        vk::RenderingAttachmentInfo attachmentInfo;
        attachmentInfo.imageView = m_Swapchain->GetImageView(m_ImageIndex);
        attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        attachmentInfo.clearValue = clearColor;
    

        // Set up the rendering info
        vk::RenderingInfo renderingInfo;
        renderingInfo.renderArea = { .offset = { 0, 0 }, .extent = m_Swapchain->GetExtent() };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &attachmentInfo;

        // Begin rendering
        cmd.beginRendering(renderingInfo);

        cmd.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_Swapchain->GetExtent().width), static_cast<float>(m_Swapchain->GetExtent().height), 0.0f, 1.0f));
		cmd.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), m_Swapchain->GetExtent()));
    }

    void VulkanGraphicsDevice::EndFrame() {
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& cmd = frame.GetCommandBuffer();

        cmd.endRendering();

        TransitionImageLayout(cmd, m_ImageIndex,
            vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite, {},
            vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe
        );

        cmd.end();

        vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        
        // Use the semaphores from the Frame object
        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &*m_Swapchain->GetImageAvailableSemaphore(m_FrameIndex);
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*cmd;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &*m_Swapchain->GetRenderFinishedSemaphore(m_ImageIndex);

        // Submit to the queue (which should be a getter in m_Device)
        m_Device->GetQueue().submit(submitInfo, *frame.GetInFlightFence());
    }

    void VulkanGraphicsDevice::Present() {
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);

        vk::PresentInfoKHR presentInfo;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &*m_Swapchain->GetRenderFinishedSemaphore(m_ImageIndex);
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &*m_Swapchain->GetSwapchain();
        presentInfo.pImageIndices = &m_ImageIndex;

        try {
            (void)m_Device->GetQueue().presentKHR(presentInfo);
        } catch (const vk::OutOfDateKHRError&) {
            int w, h;
            SDL_GetWindowSize(m_Context->GetWindow(), &w, &h);
            Resize(w, h);
        }

        m_FrameIndex = (m_FrameIndex + 1) % k_MaxFramesInFlight;
    }

    void VulkanGraphicsDevice::SetClearColor(Vec4 color) {

    }

    // Draw call
    void VulkanGraphicsDevice::SubmitDraw(IBuffer& vbo, IBuffer& ebo, IPipelineState& pipeline, uint32_t indexCount) {
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& cmd = frame.GetCommandBuffer();
        
        VulkanPipelineState* graphicsPipeline = static_cast<VulkanPipelineState*>(&pipeline);
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline->GetPipeline());

        VulkanBuffer* vertexBuffer = static_cast<VulkanBuffer*>(&vbo);
        cmd.bindVertexBuffers(0, *vertexBuffer->m_Buffer, {0});
	
        VulkanBuffer* indexBuffer = static_cast<VulkanBuffer*>(&ebo);
        cmd.bindIndexBuffer( *indexBuffer->m_Buffer, 0, vk::IndexType::eUint16 );
        
        cmd.drawIndexed(indexCount, 1, 0, 0, 0);
    }

    // Push constants
    void VulkanGraphicsDevice::PushConstants(IPipelineState& pipeline, const void* data, uint32_t size) {
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& cmd = frame.GetCommandBuffer();
        
        // We need the PipelineLayout created during Pipeline initialization
        auto* vkPipeline = static_cast<VulkanPipelineState*>(&pipeline);

        cmd.pushConstants(
            *vkPipeline->GetLayout(),
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            0,
            vk::ArrayProxy<const uint8_t>(size, static_cast<const uint8_t*>(data))
        );
    }

    // Resize
    void VulkanGraphicsDevice::Resize(int width, int height) {
        m_Swapchain->Resize(*m_Context, *m_Device, width, height);
    }

} // namespace Engine
