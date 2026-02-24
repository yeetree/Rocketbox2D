#include "Renderer/Vulkan/VulkanSwapchain.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Engine/Core/Log.h"

VulkanSwapchain::VulkanSwapchain(VulkanContext& context, VulkanDevice& device, Engine::IWindow* window) {
    CreateSwapChain(context, device, window);
    CreateImageViews(device);
    CreateSyncObjects(device);

    // Create frames
    m_Frames.clear();
    LOG_CORE_INFO("Vulkan: Creating {0} frames...", k_MaxFramesInFlight);
    
    for(uint32_t i = 0; i < k_MaxFramesInFlight; i++) {
        m_Frames.emplace_back(Engine::CreateScope<VulkanFrame>(device));
    }
}

VulkanSwapchain::~VulkanSwapchain() {
    // Nothing
}

void VulkanSwapchain::CreateSwapChain(VulkanContext& context, VulkanDevice& device, Engine::IWindow* window) {
    LOG_CORE_INFO("Vulkan: Creating Swap Chain...");

    // Get surface formats and extent
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = context.GetPhysicalDevice().getSurfaceCapabilitiesKHR( context.GetSurface() );
    m_SwapchainSurfaceFormat = ChooseSwapSurfaceFormat(context.GetPhysicalDevice().getSurfaceFormatsKHR( context.GetSurface() ));
    m_SwapchainExtent = ChooseSwapExtent(window->GetWidth(), window->GetHeight(), surfaceCapabilities);

    // Create swapchain
    vk::SwapchainCreateInfoKHR swapChainCreateInfo;
    swapChainCreateInfo.flags = vk::SwapchainCreateFlagsKHR();
    swapChainCreateInfo.surface = context.GetSurface();
    swapChainCreateInfo.minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);
    swapChainCreateInfo.imageFormat = m_SwapchainSurfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = m_SwapchainSurfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = m_SwapchainExtent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapChainCreateInfo.presentMode = ChooseSwapPresentMode(context.GetPhysicalDevice().getSurfacePresentModesKHR( context.GetSurface() ), false);
    swapChainCreateInfo.clipped = true;
    swapChainCreateInfo.oldSwapchain = nullptr;

    m_Swapchain = vk::raii::SwapchainKHR( device.GetDevice(), swapChainCreateInfo );
    m_SwapchainImages = m_Swapchain.getImages();
}

void VulkanSwapchain::CreateImageViews(VulkanDevice& device) {
    LOG_CORE_INFO("Vulkan: Creating Image Views...");
    m_SwapchainImageViews.clear();

    vk::ImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imageViewCreateInfo.format = m_SwapchainSurfaceFormat.format;
    imageViewCreateInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

    for (auto& image : m_SwapchainImages)
    {
        imageViewCreateInfo.image = image;
        m_SwapchainImageViews.emplace_back(device.GetDevice(), imageViewCreateInfo);
    }
}

void VulkanSwapchain::CreateSyncObjects(VulkanDevice& device) {
    m_ImageAvailableSemaphores.clear();
    m_RenderFinishedSemaphores.clear();
    m_ImageAvailableSemaphores.clear();
    m_RenderFinishedSemaphores.clear();
    
    uint32_t imageCount = static_cast<uint32_t>(m_SwapchainImages.size());
    
    for(uint32_t i = 0; i < imageCount; i++) {
        m_ImageAvailableSemaphores.emplace_back(device.GetDevice(), vk::SemaphoreCreateInfo());
        m_RenderFinishedSemaphores.emplace_back(device.GetDevice(), vk::SemaphoreCreateInfo());
    }
}

vk::SurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, bool vsync) {
    if(vsync) {
        return vk::PresentModeKHR::eImmediate; // Guarunteed
    }
    
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo; // Guarunteed
}

vk::Extent2D VulkanSwapchain::ChooseSwapExtent(int width, int height, const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    
    return {
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

uint32_t VulkanSwapchain::ChooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities) {
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
    {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
}

std::pair<vk::Result, uint32_t> VulkanSwapchain::AcquireNextImage(vk::raii::Semaphore& waitSem) {
    auto result = m_Swapchain.acquireNextImage(UINT64_MAX, *waitSem);
    return std::make_pair(result.result, result.value);
}

void VulkanSwapchain::Rebuild(VulkanContext& context, VulkanDevice& device, int width, int height, bool vsync) {
    if (width == 0 || height == 0) return;

    device.GetDevice().waitIdle();

    //Get capabilities and recreate swapchain
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = context.GetPhysicalDevice().getSurfaceCapabilitiesKHR(context.GetSurface());
    m_SwapchainExtent = ChooseSwapExtent(width, height, surfaceCapabilities);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo;
    swapChainCreateInfo.surface = context.GetSurface();
    swapChainCreateInfo.minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);
    swapChainCreateInfo.imageFormat = m_SwapchainSurfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = m_SwapchainSurfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = m_SwapchainExtent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapChainCreateInfo.presentMode = ChooseSwapPresentMode(context.GetPhysicalDevice().getSurfacePresentModesKHR(context.GetSurface()), vsync);
    swapChainCreateInfo.clipped = true;
    swapChainCreateInfo.oldSwapchain = *m_Swapchain;

    // Set new swapchain (raii destroys old one)
    m_Swapchain = vk::raii::SwapchainKHR(device.GetDevice(), swapChainCreateInfo);

    // Update the image list and views
    m_SwapchainImages = m_Swapchain.getImages();
    CreateImageViews(device);

    // Recreate sync objects
    for(int i = 0; i < m_Frames.size(); i++) {
        m_Frames[i]->CreateSyncObjects(device);
    }
    CreateSyncObjects(device);

    LOG_CORE_INFO("Vulkan: Swapchain resized to {0} x {1} with {2} images", 
        m_SwapchainExtent.width, m_SwapchainExtent.height, m_SwapchainImages.size());
}

VulkanFrame& VulkanSwapchain::GetFrame(uint32_t index) {
    return *m_Frames[index];
}

vk::Extent2D VulkanSwapchain::GetExtent() const {
    return m_SwapchainExtent;
}

vk::Image VulkanSwapchain::GetImage(uint32_t index) const {
    return m_SwapchainImages[index];
}

vk::raii::ImageView& VulkanSwapchain::GetImageView(uint32_t index) {
    return m_SwapchainImageViews[index];
}

vk::raii::SwapchainKHR& VulkanSwapchain::GetSwapchain() {
    return m_Swapchain;
}

vk::SurfaceFormatKHR VulkanSwapchain::GetSurfaceFormat() {
    return m_SwapchainSurfaceFormat;
}

vk::raii::Semaphore& VulkanSwapchain::GetImageAvailableSemaphore(uint32_t index) {
    return m_ImageAvailableSemaphores[index];
}

vk::raii::Semaphore& VulkanSwapchain::GetRenderFinishedSemaphore(uint32_t index) {
    return m_RenderFinishedSemaphores[index];
}