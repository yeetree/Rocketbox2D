#include "Renderer/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"
#include "Engine/Core/FileSystem.h"
#include "Renderer/Vulkan/IVulkanGraphicsBridge.h"
#include "Renderer/Vulkan/RHI/VulkanBuffer.h"
#include "Renderer/Vulkan/RHI/VulkanPipelineState.h"
#include "Renderer/Vulkan/RHI/VulkanShader.h"
#include "Renderer/Vulkan/RHI/VulkanTexture.h"
#include <SDL3/SDL_vulkan.h>

namespace Engine {

    VulkanGraphicsDevice::VulkanGraphicsDevice(IGraphicsBridge* graphicsBridge, IWindow* window)
        : m_Window(window), m_FrameIndex(0), m_ImageIndex(0) {
        LOG_CORE_INFO("Vulkan: Creating Vulkan graphics device...");

        ENGINE_CORE_ASSERT(graphicsBridge != nullptr, "Vulkan: VulkanGraphicsDevice(): graphicsBridge is nullptr!");
        ENGINE_CORE_ASSERT(window != nullptr, "Vulkan: VulkanGraphicsDevice(): window is nullptr!");
        
        // trust
        IVulkanGraphicsBridge* vkGfxBridge = static_cast<IVulkanGraphicsBridge*>(graphicsBridge);

        m_Context = CreateScope<VulkanContext>(vkGfxBridge, window);
        m_Device = CreateScope<VulkanDevice>(*m_Context);
        m_Swapchain = CreateScope<VulkanSwapchain>(*m_Context, *m_Device, window);
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice() {
        // Destructor destroys m_Swapchain before m_Device, so we wait
        // for GPU to be done before proceeding with destruction.
        m_Device->GetQueue().waitIdle();

        // Actually we kill m_Swapchain NOW
        m_Swapchain.reset();
    };

    void VulkanGraphicsDevice::TransitionImageLayout(
        vk::raii::CommandBuffer& cmd,
        vk::Image               image, // Take raw image handle
        vk::ImageLayout         oldLayout,
        vk::ImageLayout         newLayout,
        vk::AccessFlags2        srcAccess,
        vk::AccessFlags2        dstAccess,
        vk::PipelineStageFlags2 srcStage,
        vk::PipelineStageFlags2 dstStage)
    {
		vk::ImageMemoryBarrier2 barrier;
        barrier.srcStageMask        = srcStage;
        barrier.srcAccessMask       = srcAccess;
        barrier.dstStageMask        = dstStage;
        barrier.dstAccessMask       = dstAccess;
        barrier.oldLayout           = oldLayout;
        barrier.newLayout           = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = image;
        barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        vk::DependencyInfo dependencyInfo;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers    = &barrier;
        
        cmd.pipelineBarrier2(dependencyInfo);
	}

    // Resource Creation
    Scope<IBuffer> VulkanGraphicsDevice::CreateBuffer(const BufferDesc& desc) {
        return CreateScope<VulkanBuffer>(this, desc);
    }

    Scope<ITexture> VulkanGraphicsDevice::CreateTexture(const TextureDesc& desc) {
        return CreateScope<VulkanTexture>(this, desc);
    }

    Scope<IShader> VulkanGraphicsDevice::CreateShader(const ShaderDesc& desc) {
        return CreateScope<VulkanShader>(this, desc);
    }

    Scope<IPipelineState> VulkanGraphicsDevice::CreatePipelineState(const PipelineDesc& desc) {
        return CreateScope<VulkanPipelineState>(this, desc);
    }

    // Frame Management
    void VulkanGraphicsDevice::BeginFrame() {    
        // Rebuild swapchain if needed
        if(m_NeedRebuildSwapchain) {
            RebuildSwapchain();
            m_NeedRebuildSwapchain = false;
        }

        // Get frame
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& device = m_Device->GetDevice(); 

        // Wait to finish
        (void)device.waitForFences({*frame.GetInFlightFence()}, vk::True, UINT64_MAX);

        // Reset frame
        frame.Reset();

        // Get semaphore
        auto& imageAvailableSem = m_Swapchain->GetImageAvailableSemaphore(m_FrameIndex);

        // Get image index to draw on
        auto [result, imageIndex] = m_Swapchain->AcquireNextImage(imageAvailableSem);

        // Resize logic
        if (result == vk::Result::eErrorOutOfDateKHR) {
            m_NeedRebuildSwapchain = true;
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
            m_Swapchain->GetImage(m_ImageIndex),
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            {},
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput
        );

        // Set up the color attachment
        vk::ClearValue clearColor = m_ClearColor;
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

        TransitionImageLayout(cmd, m_Swapchain->GetImage(m_ImageIndex),
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

        // Reset m_CurrentPipelineState
        m_CurrentPipelineState = nullptr;
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
            m_NeedRebuildSwapchain = true;
        }

        m_FrameIndex = (m_FrameIndex + 1) % k_MaxFramesInFlight;
    }

    void VulkanGraphicsDevice::SetClearColor(Vec4 color) {
        m_ClearColor = vk::ClearColorValue(color.r, color.g, color.b, color.a);
    }

    void VulkanGraphicsDevice::SetVSync(bool vsync) {
        m_NeedRebuildSwapchain = m_VSync != vsync;
        m_VSync = vsync;
    }

    bool VulkanGraphicsDevice::IsVSync() {
        return m_VSync;
    };

    vk::raii::CommandBuffer VulkanGraphicsDevice::BeginOneTimeCommands() {
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.commandPool = m_Device->GetCommandPool();
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;
        vk::raii::CommandBuffer commandBuffer = std::move(m_Device->GetDevice().allocateCommandBuffers(allocInfo).front());

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }

    void VulkanGraphicsDevice::EndOneTimeCommands(vk::raii::CommandBuffer& commandBuffer) {
        commandBuffer.end();

        vk::SubmitInfo submitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*commandBuffer;
        m_Device->GetQueue().submit(submitInfo, nullptr);
        m_Device->GetQueue().waitIdle();
    }

    void VulkanGraphicsDevice::BindPipelineState(IPipelineState& pipeline) {
        m_CurrentPipelineState = static_cast<VulkanPipelineState*>(&pipeline);
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& cmd = frame.GetCommandBuffer();
        
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_CurrentPipelineState->GetPipeline());
    }

    // Bind vertex buffer
    void VulkanGraphicsDevice::BindVertexBuffer(IBuffer& buffer) {
        VulkanBuffer* vkBuffer = static_cast<VulkanBuffer*>(&buffer);
        m_Swapchain->GetFrame(m_FrameIndex).GetCommandBuffer().bindVertexBuffers(0, {vkBuffer->GetBuffer()}, {0});
    }

    // Bind vertex buffer
    void VulkanGraphicsDevice::BindIndexBuffer(IBuffer& buffer) {
        VulkanBuffer* vkBuffer = static_cast<VulkanBuffer*>(&buffer);
        m_Swapchain->GetFrame(m_FrameIndex).GetCommandBuffer().bindIndexBuffer(vkBuffer->GetBuffer(), 0, vk::IndexType::eUint16);
    }

    // Bind uniform buffer
    void VulkanGraphicsDevice::BindUniformBuffer(IBuffer& buffer, uint32_t slot) {
        VulkanBuffer& ubo = static_cast<VulkanBuffer&>(buffer);
        const ShaderBinding* binding = m_CurrentPipelineState->GetShaderLayout().GetBindingBySlot(slot);

        if(binding == nullptr) {
            LOG_CORE_ERROR("Vulkan: Uniform binding at slot {0} not found!", slot);
            return;
        }

        vk::DescriptorBufferInfo info{};
        info.buffer = ubo.GetBuffer();
        info.offset = 0;
        info.range  = ubo.GetSize();

        auto& key = m_PendingSets[binding->set];
        key.buffers[binding->binding] = info;
        HashCombine(key.hash, (uint64_t)(VkBuffer)info.buffer);
        HashCombine(key.hash, (uint64_t)binding->binding);
    }

    // bind texture
    void VulkanGraphicsDevice::BindTexture(ITexture& texture, uint32_t slot) {
        VulkanTexture& tex = static_cast<VulkanTexture&>(texture);
        const ShaderBinding* binding = m_CurrentPipelineState->GetShaderLayout().GetBindingBySlot(slot);
        
        if(binding == nullptr) {
            LOG_CORE_ERROR("Vulkan: Texture binding at slot {0} not found!", slot);
            return;
        }

        vk::DescriptorImageInfo info{};
        info.imageView   = tex.GetImageView();
        info.sampler     = tex.GetSampler();
        info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        auto& key = m_PendingSets[binding->set];
        key.textures[binding->binding] = info;
        HashCombine(key.hash, (uint64_t)(VkImageLayout)info.imageLayout);
        HashCombine(key.hash, (uint64_t)(VkImageView)info.imageView);
        HashCombine(key.hash, (uint64_t)(VkSampler)info.sampler);
        HashCombine(key.hash, (uint64_t)binding->binding);
    }

    // Push constants
    void VulkanGraphicsDevice::PushConstants(const void* data, uint32_t size) {
        if (!m_CurrentPipelineState) {
            LOG_CORE_WARN("Vulkan: Attempted to push constants without a bound Pipeline State!");
            return;
        }

        if(data == nullptr) {
            LOG_CORE_ERROR("Vulkan: Attempted to use invalid data with push constants!");
            return;
        }

        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& cmd = frame.GetCommandBuffer();

        cmd.pushConstants(
            *m_CurrentPipelineState->GetPipelineLayout(),
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            0,
            vk::ArrayProxy<const uint8_t>(size, static_cast<const uint8_t*>(data))
        );
    }

    // Draw call
    void VulkanGraphicsDevice::DrawIndexed(uint32_t indexCount) {
        if (!m_CurrentPipelineState) {
            LOG_CORE_WARN("Vulkan: Attempted to draw without a bound Pipeline State!");
            return;
        }
        
        auto& frame = m_Swapchain->GetFrame(m_FrameIndex);
        auto& descriptorManager = frame.GetDescriptorManager();
        auto& cmd = frame.GetCommandBuffer();

        // Get layout from pso
        std::map<uint32_t, vk::raii::DescriptorSetLayout>& layouts = m_CurrentPipelineState->GetDescriptorSetLayouts();
        
        // Get and bind pending sets
        for (auto& [setIdx, key] : m_PendingSets) {
            
            auto i = layouts.find(setIdx);
            if(i == layouts.end()) {
                LOG_CORE_WARN("Vulkan: Descriptor set layout for set {0} does not exist in current Pipeline State!", setIdx);
                continue;
            }
            vk::DescriptorSetLayout layout = m_CurrentPipelineState->GetDescriptorSetLayouts().at(setIdx);
            
            // get set from manager
            vk::DescriptorSet ds = descriptorManager.GetDescriptorSet(layout, key);

            // bind
            cmd.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                m_CurrentPipelineState->GetPipelineLayout(),
                setIdx,
                ds,
                nullptr
            );
        }
    
        // Draw
        cmd.drawIndexed(indexCount, 1, 0, 0, 0);

        // Clear pending sets
        m_PendingSets.clear();
    }

    // Resize
    void VulkanGraphicsDevice::UpdateSwapchain() {
        m_NeedRebuildSwapchain = true;
    }

    void VulkanGraphicsDevice::RebuildSwapchain() {
        m_Swapchain->Rebuild(*m_Context, *m_Device, m_Window->GetWidth(), m_Window->GetHeight(), m_VSync);
    }


    // Gives GraphicsDevice chance to finish work before app can destroy
    void VulkanGraphicsDevice::OnDestroy() {
        m_Device->GetQueue().waitIdle();
        m_PendingSets.clear();
        m_Swapchain->GetFrame(m_FrameIndex).Reset();
    }

    uint32_t VulkanGraphicsDevice::GetFrameIndex() const {
        return m_FrameIndex;
    }

    VulkanContext& VulkanGraphicsDevice::GetContext() {
        return *m_Context;
    }

    VulkanDevice& VulkanGraphicsDevice::GetDevice() {
        return *m_Device;
    }

    VulkanSwapchain& VulkanGraphicsDevice::GetSwapchain() {
        return *m_Swapchain;
    }

} // namespace Engine
