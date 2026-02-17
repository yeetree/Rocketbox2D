#include "Renderer/Vulkan/VulkanGraphicsDevice.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/FileSystem.h"
#include "Renderer/Vulkan/VulkanPipelineState.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanBuffer.h"
#include "Renderer/Vulkan/VulkanUniformBuffer.h"
#include <SDL3/SDL_vulkan.h>

namespace Engine {

    VulkanGraphicsDevice::VulkanGraphicsDevice(SDL_Window* window) : m_FrameIndex(0), m_ImageIndex(0) {
        LOG_CORE_INFO("Vulkan: Creating Vulkan graphics device...");
        
        m_Context = CreateScope<VulkanContext>(window);
        m_Device = CreateScope<VulkanDevice>(*m_Context);
        m_Swapchain = CreateScope<VulkanSwapchain>(*m_Context, *m_Device);

        // Create descriptor layout binding
        vk::DescriptorSetLayoutBinding uboBinding{};
        uboBinding.binding = 0;
        uboBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboBinding.descriptorCount = 1;
        uboBinding.stageFlags = vk::ShaderStageFlagBits::eAllGraphics;

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboBinding;

        m_UBOLayout = vk::raii::DescriptorSetLayout(m_Device->GetDevice(), layoutInfo);

        // Create descriptor pool
        vk::DescriptorPoolSize poolSize{};
        poolSize.type = vk::DescriptorType::eUniformBuffer;
        poolSize.descriptorCount = 1000; // Arbitrary big number

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.maxSets = 1000; // Arbitrary big number
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;

        m_DescriptorPool = vk::raii::DescriptorPool(m_Device->GetDevice(), poolInfo);
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

    Scope<IUniformBuffer> VulkanGraphicsDevice::CreateUniformBuffer(const UniformBufferDesc& desc) {
        return CreateScope<VulkanUniformBuffer>(this, desc);
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
        // Create the staging buffer
        BufferDesc stagingDesc;
        stagingDesc.size = size;
        stagingDesc.type = BufferType::Vertex;
        stagingDesc.isDynamic = true; // map memory
        stagingDesc.data = data; 
        VulkanBuffer stagingBuffer(this, stagingDesc);

        // Setup a command buffer to transfer
        vk::CommandBufferAllocateInfo allocInfo(*m_Device->GetCommandPool(), vk::CommandBufferLevel::ePrimary, 1);
        vk::raii::CommandBuffers cmds(m_Device->GetDevice(), allocInfo);
        vk::raii::CommandBuffer& cmd = cmds[0];

        cmd.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

        // Copy
        vk::BufferCopy copyRegion(0, dstOffset, size);
        cmd.copyBuffer(
            static_cast<vk::Buffer>(stagingBuffer.m_Buffer), 
            static_cast<vk::Buffer>(dstBuffer->m_Buffer), 
            copyRegion
        );
        cmd.end();

        // Submit and wait
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

    void VulkanGraphicsDevice::BindPipelineState(IPipelineState& pipeline) {
        m_CurrentPipelineState = static_cast<VulkanPipelineState*>(&pipeline);
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& cmd = frame.GetCommandBuffer();
        
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_CurrentPipelineState->GetPipeline());
    }

    // Draw call
    void VulkanGraphicsDevice::SubmitDraw(IBuffer& vbo, IBuffer& ebo, uint32_t indexCount) {
        if (!m_CurrentPipelineState) {
            LOG_CORE_ERROR("Vulkan: Attempted to draw without a bound Pipeline State!");
            return;
        }
        
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& cmd = frame.GetCommandBuffer();

        VulkanBuffer* vertexBuffer = static_cast<VulkanBuffer*>(&vbo);
        cmd.bindVertexBuffers(0, { static_cast<vk::Buffer>(vertexBuffer->m_Buffer) }, {0});
	
        VulkanBuffer* indexBuffer = static_cast<VulkanBuffer*>(&ebo);
        cmd.bindIndexBuffer(static_cast<vk::Buffer>(indexBuffer->m_Buffer), 0, vk::IndexType::eUint16);
        
        cmd.drawIndexed(indexCount, 1, 0, 0, 0);
    }

    // Push constants
    void VulkanGraphicsDevice::PushConstants(const void* data, uint32_t size) {
        if (!m_CurrentPipelineState) {
            LOG_CORE_ERROR("Vulkan: Attempted to push constants without a bound Pipeline State!");
            return;
        }

        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& cmd = frame.GetCommandBuffer();

        cmd.pushConstants(
            *m_CurrentPipelineState->GetLayout(),
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            0,
            vk::ArrayProxy<const uint8_t>(size, static_cast<const uint8_t*>(data))
        );
    }

    // Bind uniform buffer
    void VulkanGraphicsDevice::BindUniformBuffer(IUniformBuffer& buffer, uint32_t binding) {
        if (!m_CurrentPipelineState) {
            LOG_CORE_ERROR("Vulkan: Attempted to bind uniform buffer without a bound Pipeline State!");
            return;
        }
        
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& cmd = frame.GetCommandBuffer();

        auto* vkUniformBuffer = static_cast<VulkanUniformBuffer*>(&buffer);
        vk::DescriptorSet set = vkUniformBuffer->GetDescriptorSet(m_FrameIndex);

        cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            *m_CurrentPipelineState->GetLayout(), 
            binding,
            set,
            nullptr
        );
    }

    // Resize
    void VulkanGraphicsDevice::Resize(int width, int height) {
        m_Swapchain->Resize(*m_Context, *m_Device, width, height);
    }

    // Gives GraphicsDevice chance to finish work before app can destroy
    void VulkanGraphicsDevice::OnDestroy() {
        m_Device->GetQueue().waitIdle();
    }

    uint32_t VulkanGraphicsDevice::GetFrameIndex() {
        return m_FrameIndex;
    }

    vk::raii::DescriptorPool& VulkanGraphicsDevice::GetDescriptorPool() {
        return m_DescriptorPool;
    }

    vk::DescriptorSetLayout VulkanGraphicsDevice::GetUBODescriptorSetLayout() {
        return *m_UBOLayout;
    }

} // namespace Engine
