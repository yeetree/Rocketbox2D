#ifndef RENDERER_VULKAN_VULKANSWAPCHAIN
#define RENDERER_VULKAN_VULKANSWAPCHAIN

#include <vulkan/vulkan_raii.hpp>
    
#include "Engine/Core/Base.h"
#include "Renderer/Vulkan/VulkanFrame.h"
#include "Renderer/Vulkan/VulkanConstants.h"

// fwd
class VulkanContext;
class VulkanDevice;

class VulkanSwapchain {
public:
    // Constructor: Creates an VulkanSwapchain
    VulkanSwapchain(VulkanContext& context, VulkanDevice& device);
    ~VulkanSwapchain();

    // Prevent copying
    VulkanSwapchain(const VulkanSwapchain&) = delete;
    VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

    // Swapchain logic
    std::pair<vk::Result, uint32_t> AcquireNextImage(vk::raii::Semaphore& waitSem);

    // Getters
    VulkanFrame& GetFrame(uint32_t index);
    vk::Extent2D GetExtent() const;
    vk::Image GetImage(uint32_t index) const;
    vk::raii::ImageView& GetImageView(uint32_t index);
    vk::raii::SwapchainKHR& GetSwapchain();
    vk::SurfaceFormatKHR GetSurfaceFormat();
    vk::raii::Semaphore& GetImageAvailableSemaphore(uint32_t index);
    vk::raii::Semaphore& GetRenderFinishedSemaphore(uint32_t index);

    // Public member function to resize swapchain
    void Resize(VulkanContext& context, VulkanDevice& device, int width, int height);

private:
    // Members
    std::vector<Engine::Scope<VulkanFrame>> m_Frames; // Managed array of frames

    // Vulkan members
    std::vector<vk::raii::Semaphore> m_ImageAvailableSemaphores;
    std::vector<vk::raii::Semaphore> m_RenderFinishedSemaphores;
    vk::raii::SwapchainKHR m_Swapchain = nullptr;
    std::vector<vk::Image> m_SwapchainImages;
    vk::SurfaceFormatKHR m_SwapchainSurfaceFormat;
    vk::Extent2D m_SwapchainExtent;
    std::vector<vk::raii::ImageView> m_SwapchainImageViews;

    // Private helper functions
    void CreateSwapChain(VulkanContext& context, VulkanDevice& device);
    void CreateImageViews(VulkanDevice& device);
    void CreateSyncObjects(VulkanDevice& device);

    // Static private utility functions
    static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D ChooseSwapExtent(int width, int height, const vk::SurfaceCapabilitiesKHR& capabilities);
    static uint32_t ChooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities);
};

#endif // RENDERER_VULKAN_VULKANSWAPCHAIN
