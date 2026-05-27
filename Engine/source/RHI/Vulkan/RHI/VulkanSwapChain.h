#ifndef RHI_VULKAN_RHI_VULKANSWAPCHAIN
#define RHI_VULKAN_RHI_VULKANSWAPCHAIN

#include "Engine/RHI/ISwapChain.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "RHI/Vulkan/RHI/VulkanTexture.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    // Forward
    class VulkanGraphicsDevice;

    class ENGINE_EXPORT VulkanSwapChain : public ISwapChain {
    private:
        // Config
        uint32_t m_Width, m_Height;
        PresentMode m_Presentation;
        TextureFormat m_Format;

        // State
        bool m_RebuildSwapchain = false;

        // Vulkan
        vk::raii::SurfaceKHR m_Surface = nullptr;
        vk::Extent2D m_SwapChainExtent;
        vk::SurfaceFormatKHR m_SwapChainSurfaceFormat;
        vk::PresentModeKHR m_SwapChainPresentMode;
        vk::raii::SwapchainKHR m_SwapChain = nullptr;
        std::vector<vk::Image> m_SwapChainImages;
        std::vector<Scope<VulkanTexture>> m_SwapChainTextures;
        uint32_t m_AcquiredImageIndex = 0;

        // Sync Objects
        std::vector<vk::raii::Semaphore> m_PresentCompleteSemaphores; // OS gives us backbuffer - Global frame index
        std::vector<vk::raii::Semaphore> m_RenderFinishedSemaphores; // Finished drawing - Acquired image index

        // Helpers
        vk::PresentModeKHR GetPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, PresentMode presentation);
        vk::SurfaceFormatKHR GetSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats, TextureFormat format);
        vk::Extent2D GetExtent(uint32_t width, uint32_t height, const vk::SurfaceCapabilitiesKHR& capabilities);
        uint32_t GetMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities);

        void BuildSwapChain(VulkanContext* context);

    public:
        VulkanSwapChain(VulkanContext* context, IVulkanGraphicsBridge* bridge, const SwapChainDesc& desc);
        ~VulkanSwapChain() = default;

        ITexture* GetCurrentBackBuffer()
        {
            return m_SwapChainTextures[m_AcquiredImageIndex].get();
        };

        // Vulkan

        // SwapChain config
        void Resize(uint32_t width, uint32_t height); // Called on window resize events
        void SetPresentation(PresentMode presentation);

        void AcquireNextImage(VulkanContext* context, uint32_t frameIdx);
        void Present(VulkanContext* context, uint32_t frameIdx);

        vk::raii::Semaphore& GetPresentCompleteSemaphore(uint32_t frameIdx)
        {
            return m_PresentCompleteSemaphores[frameIdx];
        }

        vk::raii::Semaphore& GetRenderFinishedSemaphore()
        {
            return m_RenderFinishedSemaphores[m_AcquiredImageIndex];
        }
    };
} // namespace Engine


#endif // RHI_VULKAN_RHI_VULKANSWAPCHAIN
