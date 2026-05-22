#include "RHI/Vulkan/VulkanSwapchain.h"
#include "RHI/Vulkan/VulkanConstants.h"

#include "Engine/Core/Log.h"

namespace Engine
{
    VulkanSwapchain::VulkanSwapchain(Ref<VulkanContext> context, const SwapchainDesc& desc) 
        : m_Context(context), m_Width(desc.width), m_Height(desc.height), m_Format(desc.format), m_Presentation(desc.presentation),
          m_RebuildSwapchain(true)
    {
        BuildSwapchain();
    }

    void VulkanSwapchain::Resize(uint32_t width, uint32_t height)
    {
        if(m_Width != width || m_Height != height)
        {
            m_Width = width;
            m_Height = height;
            m_RebuildSwapchain = true;
        }
    }

    void VulkanSwapchain::SetPresentation(PresentMode presentation)
    {
        if(m_Presentation != presentation)
        {
            m_Presentation = presentation;
            m_RebuildSwapchain = true;
        }
    }

    vk::PresentModeKHR VulkanSwapchain::GetPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, PresentMode presentation)
    {
        // Get desired presentation mode
        vk::PresentModeKHR pmode = vk::PresentModeKHR::eFifo;
        switch(presentation)
        {
            case PresentMode::Immediate: pmode = vk::PresentModeKHR::eImmediate; break;
            case PresentMode::Mailbox:   pmode = vk::PresentModeKHR::eMailbox; break;
            case PresentMode::VSync:     pmode = vk::PresentModeKHR::eFifo; break;
        }

        // Check if it is available
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == pmode) {
                return pmode;
            }
        }

        // Else, return default VSync, guarunteed.
        return vk::PresentModeKHR::eFifo;
    }

    vk::SurfaceFormatKHR VulkanSwapchain::GetSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats, TextureFormat format)
    {
        vk::Format surfacePixelFormat = vk::Format::eR8G8B8A8Srgb;;
        vk::ColorSpaceKHR surfaceColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;;

        // Get desired format
        switch(format)
        {
            case TextureFormat::RGBA8:
            {
                surfacePixelFormat = vk::Format::eR8G8B8A8Srgb;
                surfaceColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
                break;
            }
        }

        // Check if it is available
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == surfacePixelFormat && availableFormat.colorSpace == surfaceColorSpace) {
                return availableFormat;
            }
        }

        // Else, return default: first format available
        return availableFormats[0];
    }

    vk::Extent2D VulkanSwapchain::GetExtent(uint32_t width, uint32_t height, const vk::SurfaceCapabilitiesKHR& capabilities) 
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        return {
            std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }

    uint32_t VulkanSwapchain::GetMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
    {
        uint32_t minImageCount = std::max(3u, capabilities.minImageCount);
        if ((0 < capabilities.maxImageCount) && (capabilities.maxImageCount < minImageCount))
        {
            minImageCount = capabilities.maxImageCount;
        }
        return minImageCount;
    }

    void VulkanSwapchain::BuildSwapchain()
    {

        vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_Context->GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_Context->GetSurface());
        if(surfaceCapabilities.currentExtent.width == 0 || surfaceCapabilities.currentExtent.height == 0)
        {
            // We can't
            return;
        }

        LOG_CORE_INFO("Vulkan: Creating Swapchain...");

        m_Context->GetDevice().waitIdle();

        m_FrameCommandBuffers.clear();
        m_FrameCommandPools.clear();
        
        m_ImageAvailableSemaphores.clear();
        m_RenderFinishedSemaphores.clear();
        m_InFlightFences.clear();
        m_SwapchainImageViews.clear();
        m_SwapchainImages.clear();

        // Get surface format and extent
        m_SwapchainSurfaceFormat = GetSurfaceFormat(m_Context->GetPhysicalDevice().getSurfaceFormatsKHR(m_Context->GetSurface()), m_Format);
        m_SwapchainExtent = GetExtent(m_Width, m_Height, surfaceCapabilities);

        // Create swapchain
        // Build swapchain create info with correct parameter types
        vk::SwapchainCreateInfoKHR swapChainCreateInfo(
            vk::SwapchainCreateFlagsKHR(),
            m_Context->GetSurface(),
            GetMinImageCount(surfaceCapabilities),
            m_SwapchainSurfaceFormat.format,
            m_SwapchainSurfaceFormat.colorSpace,
            m_SwapchainExtent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            0,
            nullptr,
            surfaceCapabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            GetPresentMode(m_Context->GetPhysicalDevice().getSurfacePresentModesKHR(m_Context->GetSurface()), m_Presentation),
            VK_TRUE,
            *m_Swapchain
        );

        m_Swapchain = vk::raii::SwapchainKHR( m_Context->GetDevice(), swapChainCreateInfo );
        m_SwapchainImages = m_Swapchain.getImages();

        vk::ImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = m_SwapchainSurfaceFormat.format;
        imageViewCreateInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

        for (auto& image : m_SwapchainImages)
        {
            imageViewCreateInfo.image = image;
            m_SwapchainImageViews.emplace_back(m_Context->GetDevice(), imageViewCreateInfo);
        }
        
        for(uint32_t i = 0; i < k_MaxFramesInFlight; i++)
        {
            vk::CommandPoolCreateInfo poolInfo(
                vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                m_Context->GetGraphicsQueue().familyIndex
            );
            m_FrameCommandPools.emplace_back(m_Context->GetDevice(), poolInfo);
            m_FrameCommandBuffers.push_back(CreateScope<VulkanCommandBuffer>(m_Context, m_FrameCommandPools.back()));

            m_ImageAvailableSemaphores.emplace_back(m_Context->GetDevice(), vk::SemaphoreCreateInfo());
            m_RenderFinishedSemaphores.emplace_back(m_Context->GetDevice(), vk::SemaphoreCreateInfo());
            
            // Signaled to not block first BeginFrame
            vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);
            m_InFlightFences.emplace_back(m_Context->GetDevice(), fenceInfo);
        }

        m_RebuildSwapchain = false;
        m_CurrentFrameIndex = 0;
    }

    ICommandBuffer* VulkanSwapchain::BeginFrame() {
        if (m_Width == 0 || m_Height == 0)
        {
            return nullptr;
        }

        auto& device = m_Context->GetDevice();


        if (m_RebuildSwapchain)
        {
            BuildSwapchain();
            if(m_RebuildSwapchain) {
                return nullptr; // If we still need to rebuild, then we must skip.
            }
        }

        (void)device.waitForFences({*m_InFlightFences[m_CurrentFrameIndex]}, true, UINT64_MAX);

        try 
        {
            auto acquireResult = m_Swapchain.acquireNextImage(
                UINT64_MAX, 
                *m_ImageAvailableSemaphores[m_CurrentFrameIndex], 
                nullptr
            );
            m_CurrentImageIndex = acquireResult.value;
        }
        catch (const vk::OutOfDateKHRError&) 
        {
            m_RebuildSwapchain = true;
            return nullptr; 
        }

        device.resetFences({*m_InFlightFences[m_CurrentFrameIndex]});

        auto* cmd = m_FrameCommandBuffers[m_CurrentFrameIndex].get();
        cmd->Begin();

        vk::ImageMemoryBarrier barrier;
        barrier.oldLayout = vk::ImageLayout::eUndefined;
        barrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
        barrier.srcAccessMask = vk::AccessFlags();
        barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        barrier.image = m_SwapchainImages[m_CurrentImageIndex];
        barrier.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

        cmd->GetCommandBuffer().pipelineBarrier(
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::DependencyFlags(),
            nullptr, nullptr, barrier
        );

        return cmd;
    }

    void VulkanSwapchain::EndFrame(ICommandBuffer* cmd) {
        if(cmd == nullptr) { return; }

        auto* vulkanCmd = static_cast<VulkanCommandBuffer*>(cmd);
        auto& rawCmd = vulkanCmd->GetCommandBuffer();

        vk::ImageMemoryBarrier barrier;
        barrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
        barrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
        barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
        barrier.image = m_SwapchainImages[m_CurrentImageIndex];
        barrier.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

        rawCmd.pipelineBarrier(
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::DependencyFlags(),
            nullptr, nullptr, barrier
        );

        vulkanCmd->End();

        vk::Semaphore waitSemaphores[]   = { *m_ImageAvailableSemaphores[m_CurrentFrameIndex] };
        vk::Semaphore signalSemaphores[] = { *m_RenderFinishedSemaphores[m_CurrentImageIndex] };
        
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        vk::CommandBuffer rawCmdBufferHandle = *rawCmd;

        vk::SubmitInfo submitInfo(1, waitSemaphores, waitStages, 1, &rawCmdBufferHandle, 1, signalSemaphores);
        m_Context->GetGraphicsQueue().queue.submit(submitInfo, *m_InFlightFences[m_CurrentFrameIndex]);

        vk::SwapchainKHR swapchains[] = { *m_Swapchain };
        vk::PresentInfoKHR presentInfo(1, signalSemaphores, 1, swapchains, &m_CurrentImageIndex);

        try
        {
            m_Context->GetGraphicsQueue().queue.presentKHR(presentInfo);
        }
        catch (const vk::OutOfDateKHRError&)
        {
            m_RebuildSwapchain = true;
        }

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % k_MaxFramesInFlight;
    }
} // namespace Engine
