#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "RHI/Vulkan/VulkanConstants.h"
#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "RHI/Vulkan/VulkanFrame.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "Engine/Platform/IWindow.h"
#include "Engine/Core/Assert.h"
#include "Engine/Core/Log.h"

namespace Engine::RHI::Vulkan
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(Scope<IVulkanGraphicsBridge> bridge)
        : m_Bridge(std::move(bridge)),
          m_Context(m_Bridge.get()),
          m_ImmediateAllocator(m_Context),
          m_FrameIndex(0)
    {
        // Create frames
        for(uint32_t i = 0; i < k_MaxFramesInFlight; i++)
        {
            m_Frames.push_back(CreateScope<VulkanFrame>(m_Context));
        }
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice() = default;

    // Resource creation
    BufferHandle VulkanGraphicsDevice::CreateBuffer(const BufferDesc& desc)
    {
        return { .id = 0 };
    }

    TextureHandle VulkanGraphicsDevice::CreateTexture(const TextureDesc& desc)
    {
        return { .id = 0 };
    }

    ShaderHandle VulkanGraphicsDevice::CreateShader(const ShaderDesc& desc)
    {
        return { .id = 0 };
    }

    PipelineHandle VulkanGraphicsDevice::CreatePipeline(const PipelineDesc& desc)
    {
        return { .id = 0 };
    }

    SwapChainHandle VulkanGraphicsDevice::CreateSwapChain(const SwapChainDesc& desc)
    {
        ENGINE_CORE_ASSERT(desc.window != nullptr, "Vulkan: VulkanGraphicsDevice: CreateSwapChain(): desc.window is nullptr!");

        // Get data
        uint32_t id = AllocateID();
        VulkanSwapChainData& data = m_SwapChains[id]; 

        // Create surface
        vk::SurfaceKHR surface = m_Bridge->CreateSurface(
            m_Context.GetInstance(),
            m_Context.GetPhysicalDevice(),
            m_Context.GetGraphicsQueue().familyIndex,
            desc.window
        );
        data.surface = vk::raii::SurfaceKHR(m_Context.GetInstance(), surface);

        // Configuration
        data.format = desc.format;
        data.presentMode = desc.presentation;
        data.width = desc.window->GetWidth();
        data.height = desc.window->GetHeight();

        RebuildSwapchain(data);

        return SwapChainHandle{ .id = id };
    }

    // Resource destruction
    void VulkanGraphicsDevice::DestroyBuffer(BufferHandle& buffer)
    {
        VulkanBufferData& data = GetBufferData(buffer);
        m_Buffers.erase(buffer.id);
        buffer.id = 0;
    }

    void VulkanGraphicsDevice::DestroyTexture(TextureHandle& texture)
    {
        VulkanTextureData& data = GetTextureData(texture);
        m_Textures.erase(texture.id);
        texture.id = 0;
    }

    void VulkanGraphicsDevice::DestroyShader(ShaderHandle& shader)
    {
        VulkanShaderData& data = GetShaderData(shader);
        m_Shaders.erase(shader.id);
        shader.id = 0;
    }

    void VulkanGraphicsDevice::DestroyPipeline(PipelineHandle& pipeline)
    {
        VulkanPipelineData& data = GetPipelineData(pipeline);
        m_Pipelines.erase(pipeline.id);
        pipeline.id = 0;
    }

    void VulkanGraphicsDevice::DestroySwapChain(SwapChainHandle& swapchain)
    {
        VulkanSwapChainData& data = GetSwapChainData(swapchain);
        m_SwapChains.erase(swapchain.id);
        swapchain.id = 0;
    }

    // Frame pacing
    void VulkanGraphicsDevice::BeginFrame()
    {
        // Wait for fence
        m_Context.GetDevice().waitForFences(*m_Frames[m_FrameIndex]->GetFence(), VK_TRUE, UINT64_MAX);
        m_Context.GetDevice().resetFences(*m_Frames[m_FrameIndex]->GetFence());
        m_Frames[m_FrameIndex]->Reset();

        // Clear previous submission info
        m_FrameSubmits.clear();
        m_FrameCommandBuffers.clear();
        m_FrameWaitSemaphores.clear();
        m_FrameSignalSemaphores.clear();
        m_FrameStageFlags.clear();
        m_FrameSwapChainPresentations.clear();
    }

    void VulkanGraphicsDevice::EndFrame()
    {
        // Submit everything
        m_Context.GetGraphicsQueue().queue.submit(
            m_FrameSubmits,
            *m_Frames[m_FrameIndex]->GetFence()
        );

        // Present all swapchains that were rendered to
        for (SwapChainHandle handle : m_FrameSwapChainPresentations)
        {
            VulkanSwapChainData& sc = GetSwapChainData(handle);

            vk::PresentInfoKHR presentInfo;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = &*sc.renderFinishedSemaphores[sc.acquiredImageIndex];
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &*sc.swapchain;
            presentInfo.pImageIndices      = &sc.acquiredImageIndex;

            try
            {
                m_Context.GetGraphicsQueue().queue.presentKHR(presentInfo);
            }
            catch (const vk::OutOfDateKHRError&)
            {
                sc.needsRebuild = true;
            }
        }

        // Advance frame
        m_FrameIndex = (m_FrameIndex + 1) % k_MaxFramesInFlight;
    }

    // Render passes
    //ICommandBuffer* VulkanGraphicsDevice::BeginPass(TextureHandle renderTarget, Vec4 clearColor)
    //{
    
    //}

    ICommandBuffer* VulkanGraphicsDevice::BeginPass(SwapChainHandle renderTarget, Vec4 clearColor)
    {
        // Get swapchain
        VulkanSwapChainData& sc = GetSwapChainData(renderTarget);
        
        // Rebuild
        if(sc.needsRebuild)
        {
            RebuildSwapchain(sc);
        }

        // If we still need to rebuild, then we return nullptr
        if(sc.needsRebuild)
        {
            return nullptr;
        }

        

        // Acquire next image
        sc.acquiredImageIndex = sc.swapchain.acquireNextImage(
            UINT64_MAX,
            sc.presentCompleteSemaphores[m_FrameIndex],
            nullptr
        ).value;

        // Get command buffer and begin rendering
        VulkanCommandBuffer* cmd = m_Frames[m_FrameIndex]->GetCommandBufferAllocator().GetOrAllocate(*this);
        cmd->BeginRendering(&sc.textures[sc.acquiredImageIndex], clearColor);
        m_FrameSwapChainPresentations.push_back(renderTarget);

        return cmd;
    }

    void VulkanGraphicsDevice::EndPass(ICommandBuffer* cmd)
    {
        ENGINE_ASSERT(cmd != nullptr, "Vulkan: VulkanGraphicsDevice: EndPass(): cmd is nullptr!");

        // End command buffer
        VulkanCommandBuffer* vcmd = static_cast<VulkanCommandBuffer*>(cmd);
        vcmd->EndRendering();

        // Swapchain data
        SwapChainHandle sc = m_FrameSwapChainPresentations.back();
        VulkanSwapChainData& scData = GetSwapChainData(sc);

        // Semaphores
        vk::Semaphore presentComplete = *scData.presentCompleteSemaphores[m_FrameIndex];
        vk::Semaphore renderFinished  = *scData.renderFinishedSemaphores[scData.acquiredImageIndex];

        // Submit info
        m_FrameCommandBuffers.push_back(*vcmd->GetCommandBuffer());
        m_FrameWaitSemaphores.push_back(presentComplete);
        m_FrameSignalSemaphores.push_back(renderFinished);
        m_FrameStageFlags.push_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);

        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = &m_FrameWaitSemaphores.back();
        submitInfo.pWaitDstStageMask    = &m_FrameStageFlags.back();
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &m_FrameCommandBuffers.back();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &m_FrameSignalSemaphores.back();

        m_FrameSubmits.push_back(submitInfo);
    }

    // Immediate command buffer
    ICommandBuffer* VulkanGraphicsDevice::BeginImmediate()
    {
        return nullptr;
    }

    void VulkanGraphicsDevice::EndImmediate(ICommandBuffer* cmd)
    {

    }

    // Swapchain configuration
    void VulkanGraphicsDevice::ResizeSwapChain(SwapChainHandle swapchain, uint32_t width, uint32_t height)
    {
        // Get swapchain
        VulkanSwapChainData& sc = GetSwapChainData(swapchain);

        if(sc.width != width || sc.height != height)
        {
            sc.width = width;
            sc.height = height;
            sc.needsRebuild = true;
        }
    } 

    void VulkanGraphicsDevice::SetSwapChainPresentMode(SwapChainHandle swapchain, PresentMode mode)
    {
        // Get swapchain
        VulkanSwapChainData& sc = GetSwapChainData(swapchain);

        if(sc.presentMode != mode)
        {
            sc.presentMode = mode;
            sc.needsRebuild = true;
        }
    }


    // Destroy
    void VulkanGraphicsDevice::OnDestroy()
    {
        m_Context.GetDevice().waitIdle();
    }

    // Swapchains
    void VulkanGraphicsDevice::RebuildSwapchain(VulkanSwapChainData& swapChainData)
    {
        m_Context.GetDevice().waitIdle();

        vk::SurfaceCapabilitiesKHR capabilities = m_Context.GetPhysicalDevice().getSurfaceCapabilitiesKHR(*swapChainData.surface);

        // Extent
        swapChainData.extent = VulkanCommon::GetSurfaceExtent(
            capabilities,
            swapChainData.width,
            swapChainData.height
        );

        // Skip if width or height is 0
        if (swapChainData.extent.width == 0 || swapChainData.extent.height == 0)
            return;

        // Clear
        swapChainData.renderFinishedSemaphores.clear();
        swapChainData.presentCompleteSemaphores.clear();
        swapChainData.textures.clear();
        swapChainData.images.clear();

        // Format
        swapChainData.surfaceFormat = VulkanCommon::ChooseSurfaceFormat(
            m_Context.GetPhysicalDevice().getSurfaceFormatsKHR(*swapChainData.surface),
            VulkanCommon::GetSurfaceFormat(swapChainData.format)
        );
        ENGINE_CORE_ASSERT(swapChainData.surfaceFormat.format != vk::Format::eUndefined, "Vulkan: VulkanGraphicsDevice: CreateSwapChain(): Surface format is not supported!");
        
        // Present mode
        swapChainData.vkPresentMode = VulkanCommon::ChoosePresentMode(
            m_Context.GetPhysicalDevice().getSurfacePresentModesKHR(*swapChainData.surface),
            VulkanCommon::GetPresentMode(swapChainData.presentMode)
        );
        if(swapChainData.vkPresentMode != VulkanCommon::GetPresentMode(swapChainData.presentMode))
        {
            LOG_CORE_WARN("Vulkan: VulkanGraphicsDevice: CreateSwapChain(): Present mode is unsupported. Falling back to PresentMode::Immediate");
            swapChainData.vkPresentMode = vk::PresentModeKHR::eImmediate;
            swapChainData.presentMode = PresentMode::Immediate;
        }

        // Min images
        uint32_t minImages = VulkanCommon::GetSurfaceMinImageCount(capabilities);

        // Swapchain
        vk::SwapchainCreateInfoKHR swapChainCreateInfo(
            {},
            *swapChainData.surface,
            minImages,
            swapChainData.surfaceFormat.format,
            swapChainData.surfaceFormat.colorSpace,
            swapChainData.extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            0, nullptr,
            capabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            swapChainData.vkPresentMode,
            true,
            *swapChainData.swapchain
        );
        
        swapChainData.swapchain = vk::raii::SwapchainKHR(m_Context.GetDevice(), swapChainCreateInfo);
        swapChainData.images = swapChainData.swapchain.getImages();

        // Textures
        TextureDesc texDesc{
            .width = swapChainData.extent.width,
            .height = swapChainData.extent.height,
            .format = swapChainData.format,
            .usage = TextureUsage::RenderTarget
        };

        // Semaphores
        vk::SemaphoreCreateInfo semaphoreCreateInfo{};

        // Textures and RenderFinishedSemaphores: per swapchain image
        for(auto &image : swapChainData.images)
        {
            // Create texture
            VulkanTextureData& texData = swapChainData.textures.emplace_back();
            texData.desc = texDesc;
            texData.format = swapChainData.surfaceFormat.format; // format selection guaruntees that vk::SurfaceFormatKHR and PixelFormat match.
                                                                 // desc.format isn't even used here anyway.
            texData.image = image;
            texData.ownsImage = false;

            CreateImageView(texData);

            // Create new render finished semaphore
            swapChainData.renderFinishedSemaphores.emplace_back(m_Context.GetDevice(), semaphoreCreateInfo);
        }

        // PresentCompleteSemaphores: per global frame
        for(int i = 0; i < k_MaxFramesInFlight; i++)
        {
            swapChainData.presentCompleteSemaphores.emplace_back(m_Context.GetDevice(), semaphoreCreateInfo);
        }

        swapChainData.needsRebuild = false;
    }

    void VulkanGraphicsDevice::CreateImageView(VulkanTextureData& textureData)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo(
            {},
            textureData.image,
            vk::ImageViewType::e2D,
            textureData.format,
            {},
            {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
        );

        textureData.imageView = vk::raii::ImageView(m_Context.GetDevice(), imageViewCreateInfo);
    }

    // Public getters for Vulkan classes
    VulkanBufferData& VulkanGraphicsDevice::GetBufferData(BufferHandle buffer)
    {
        ENGINE_CORE_ASSERT(buffer.IsValid(), "Vulkan: VulkanGraphicsDevice: GetBufferData: buffer is invalid!");
    
        auto it = m_Buffers.find(buffer.id);
        ENGINE_ASSERT(it != m_Buffers.end(), "Vulkan: VulkanGraphicsDevice: GetBufferData: buffer is not found!");
        
        return it->second;
    }

    VulkanTextureData& VulkanGraphicsDevice::GetTextureData(TextureHandle texture)
    {
        ENGINE_CORE_ASSERT(texture.IsValid(), "Vulkan: VulkanGraphicsDevice: GetTextureData: texture is invalid!");
    
        auto it = m_Textures.find(texture.id);
        ENGINE_ASSERT(it != m_Textures.end(), "Vulkan: VulkanGraphicsDevice: GetTextureData: texture is not found!");
        
        return it->second;
    }

    VulkanShaderData& VulkanGraphicsDevice::GetShaderData(ShaderHandle shader)
    {
        ENGINE_CORE_ASSERT(shader.IsValid(), "Vulkan: VulkanGraphicsDevice: GetShaderData: shader is invalid!");
    
        auto it = m_Shaders.find(shader.id);
        ENGINE_ASSERT(it != m_Shaders.end(), "Vulkan: VulkanGraphicsDevice: GetShaderData: shader is not found!");
        
        return it->second;
    }

    VulkanPipelineData& VulkanGraphicsDevice::GetPipelineData(PipelineHandle pipeline)
    {
        ENGINE_CORE_ASSERT(pipeline.IsValid(), "Vulkan: VulkanGraphicsDevice: GetPipelineData: pipeline is invalid!");
    
        auto it = m_Pipelines.find(pipeline.id);
        ENGINE_ASSERT(it != m_Pipelines.end(), "Vulkan: VulkanGraphicsDevice: GetPipelineData: pipeline is not found!");
        
        return it->second;
    }

    VulkanSwapChainData& VulkanGraphicsDevice::GetSwapChainData(SwapChainHandle swapchain)
    {
        ENGINE_CORE_ASSERT(swapchain.IsValid(), "Vulkan: VulkanGraphicsDevice: GetSwapChainData: swapchain is invalid!");
    
        auto it = m_SwapChains.find(swapchain.id);
        ENGINE_ASSERT(it != m_SwapChains.end(), "Vulkan: VulkanGraphicsDevice: GetSwapChainData: swapchain is not found!");
        
        return it->second;
    }
} // namespace Engine::RHI::Vulkan
