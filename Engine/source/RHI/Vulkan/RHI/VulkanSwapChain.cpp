#include "RHI/Vulkan/RHI/VulkanSwapChain.h"
#include "RHI/Vulkan/VulkanConstants.h"

#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanSwapChain::VulkanSwapChain(VulkanContext* context, IVulkanGraphicsBridge* bridge, const SwapChainDesc& desc)
        : m_Context(context), m_Format(desc.format), m_Presentation(desc.presentation), m_RebuildSwapchain(true)
    {
        LOG_CORE_INFO("Vulkan: Creating swapchain...");

        ENGINE_CORE_ASSERT(m_Context != nullptr, "Vulkan: VulkanSwapChain(): context is nullptr!");
        ENGINE_CORE_ASSERT(bridge != nullptr, "Vulkan: VulkanSwapChain(): bridge is nullptr!");
        ENGINE_CORE_ASSERT(desc.window != nullptr, "Vulkan: VulkanSwapChain(): window is nullptr!");

        m_Width = desc.window->GetWidth();
        m_Height = desc.window->GetHeight();

        // Create swapchain
        VkSurfaceKHR surf = bridge->CreateSurface(
            *m_Context->GetInstance(),
            *m_Context->GetPhysicalDevice(),
            m_Context->GetGraphicsQueue().familyIndex,
            desc.window
        );

        m_Surface = vk::raii::SurfaceKHR(m_Context->GetInstance(), surf, nullptr);

        BuildSwapChain();
    }

    void VulkanSwapChain::BuildSwapChain()
    {
        LOG_CORE_INFO("Vulkan: Building swapchain...");

        m_Context->GetDevice().waitIdle();

        // Clear
        m_RenderFinishedSemaphores.clear();
        m_PresentCompleteSemaphores.clear();
        m_SwapChainTextures.clear();
        m_SwapChainImages.clear();

        // Build SwapChain
        vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_Context->GetPhysicalDevice().getSurfaceCapabilitiesKHR( *m_Surface );
        
        m_SwapChainExtent = GetExtent(m_Width, m_Height, surfaceCapabilities);
        
        uint32_t minImageCount = GetMinImageCount(surfaceCapabilities);

        std::vector<vk::SurfaceFormatKHR> availableFormats = m_Context->GetPhysicalDevice().getSurfaceFormatsKHR(*m_Surface);
        m_SwapChainSurfaceFormat = GetSurfaceFormat(availableFormats, m_Format);

        std::vector<vk::PresentModeKHR> availableModes = m_Context->GetPhysicalDevice().getSurfacePresentModesKHR(*m_Surface);
        m_SwapChainPresentMode = GetPresentMode(availableModes, m_Presentation);

        vk::SwapchainCreateInfoKHR swapChainCreateInfo(
            {},
            *m_Surface,
            minImageCount,
            m_SwapChainSurfaceFormat.format,
            m_SwapChainSurfaceFormat.colorSpace,
            m_SwapChainExtent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            0, nullptr,
            surfaceCapabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            m_SwapChainPresentMode,
            true,
            *m_SwapChain
        );
        
        m_SwapChain = vk::raii::SwapchainKHR(m_Context->GetDevice(), swapChainCreateInfo);
        m_SwapChainImages = m_SwapChain.getImages(); 

        // Get textures
        TextureDesc texDesc{
            .width = m_Width,
            .height = m_Height,
            .format = m_Format,
            .usage = TextureUsage::RenderTarget
        };

        for(auto &image : m_SwapChainImages)
        {
            m_SwapChainTextures.push_back(
                std::move(
                    CreateScope<VulkanTexture>(m_Context, image, m_SwapChainSurfaceFormat.format, texDesc)
                )
            );
        }

        vk::SemaphoreCreateInfo semaphoreCreateInfo{};

        // Create semaphores
        for(int i = 0; i < k_MaxFramesInFlight; i++)
        {
            m_PresentCompleteSemaphores.emplace_back(m_Context->GetDevice(), semaphoreCreateInfo);
            m_RenderFinishedSemaphores.emplace_back(m_Context->GetDevice(), semaphoreCreateInfo);
        }
    }

    void VulkanSwapChain::Resize(uint32_t width, uint32_t height)
    {
        if(m_Width != width || m_Height != height)
        {
            m_Width = width;
            m_Height = height;
            m_RebuildSwapchain = true;
        }
    }

    void VulkanSwapChain::SetPresentation(PresentMode presentation)
    {
        if(m_Presentation != presentation)
        {
            m_Presentation = presentation;
            m_RebuildSwapchain = true;
        }
    }

    void VulkanSwapChain::AcquireNextImage(uint32_t frameIdx)
    {
        m_AcquiredImageIndex = m_SwapChain.acquireNextImage(
            UINT64_MAX,
            m_PresentCompleteSemaphores[frameIdx],
            nullptr
        ).value;
    }

    void VulkanSwapChain::Present(uint32_t frameIdx)
    {
        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &*(m_RenderFinishedSemaphores[frameIdx]);
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &*m_SwapChain;
        presentInfo.pImageIndices = &m_AcquiredImageIndex;

        try
        {
            m_Context->GetGraphicsQueue().queue.presentKHR(presentInfo);
        }
        catch (const vk::OutOfDateKHRError&)
        {
            m_RebuildSwapchain = true;
        }
    }


    vk::PresentModeKHR VulkanSwapChain::GetPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, PresentMode presentation)
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

    vk::SurfaceFormatKHR VulkanSwapChain::GetSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats, TextureFormat format)
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

    vk::Extent2D VulkanSwapChain::GetExtent(uint32_t width, uint32_t height, const vk::SurfaceCapabilitiesKHR& capabilities) 
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        return {
            std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }

    uint32_t VulkanSwapChain::GetMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
    {
        uint32_t minImageCount = std::max((uint32_t)k_MaxFramesInFlight, capabilities.minImageCount);
        if ((0 < capabilities.maxImageCount) && (capabilities.maxImageCount < minImageCount))
        {
            minImageCount = capabilities.maxImageCount;
        }
        return minImageCount;
    }
}