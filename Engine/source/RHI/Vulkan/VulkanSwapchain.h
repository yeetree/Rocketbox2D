#ifndef RHI_VULKAN_VULKANSWAPCHAIN
#define RHI_VULKAN_VULKANSWAPCHAIN

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/ISwapchain.h"

#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanCommandBuffer.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    class ENGINE_EXPORT VulkanSwapchain : public ISwapchain
    {
    private:
        // Config
        uint32_t m_Width, m_Height;
        PresentMode m_Presentation;
        TextureFormat m_Format;

        // State
        bool m_RebuildSwapchain = false;
        Ref<VulkanContext> m_Context;
        vk::raii::SwapchainKHR m_Swapchain = nullptr;
        vk::SurfaceFormatKHR m_SwapchainSurfaceFormat;
        vk::Extent2D m_SwapchainExtent;
        std::vector<vk::Image> m_SwapchainImages;
        std::vector<vk::raii::ImageView> m_SwapchainImageViews;

        uint32_t m_CurrentFrameIndex = 0;
        uint32_t m_CurrentImageIndex = 0;

        std::vector<vk::raii::CommandPool> m_FrameCommandPools;
        std::vector<Scope<VulkanCommandBuffer>> m_FrameCommandBuffers;
        std::vector<vk::raii::Semaphore> m_ImageAvailableSemaphores;
        std::vector<vk::raii::Semaphore> m_RenderFinishedSemaphores;
        std::vector<vk::raii::Fence> m_InFlightFences;

        // Helpers
        void BuildSwapchain();

        // Static helpers
        static vk::PresentModeKHR GetPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, PresentMode presentation);
        static vk::SurfaceFormatKHR GetSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& avaliableFormats, TextureFormat format);
        static vk::Extent2D GetExtent(uint32_t width, uint32_t height, const vk::SurfaceCapabilitiesKHR& capabilities);
        static uint32_t GetMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities);

    public:
        VulkanSwapchain(Ref<VulkanContext> context, const SwapchainDesc& desc);
        ~VulkanSwapchain() override = default;

        // Swapchain config
        void Resize(uint32_t width, uint32_t height) override; // Called on window resize events
        void SetPresentation(PresentMode presentation) override;

        // Blocks until GPU is ready to begin drawing
        ICommandBuffer* BeginFrame() override;

        // Presents image, begins working on next.
        void EndFrame(ICommandBuffer* cmd) override;
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANSWAPCHAIN
