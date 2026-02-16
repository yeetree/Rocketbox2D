#ifndef RENDERER_VULKAN_VULKANFRAME
#define RENDERER_VULKAN_VULKANFRAME

#include <vulkan/vulkan_raii.hpp>

// fwd
class VulkanDevice;

class VulkanFrame {
public:
    // Constructor: Creates a VulkanFrame
    VulkanFrame(VulkanDevice& device);
    ~VulkanFrame();

    // Prevent copying
    VulkanFrame(const VulkanFrame&) = delete;
    VulkanFrame& operator=(const VulkanFrame&) = delete;

    // Getters
    vk::raii::Semaphore& GetImageAvailableSemaphore();
    vk::raii::Semaphore& GetRenderFinishedSemaphore();
    vk::raii::Fence& GetInFlightFence();
    vk::raii::CommandBuffer& GetCommandBuffer();

private:
    // Vulkan members
    vk::raii::Semaphore m_ImageAvailableSemaphore = nullptr;
    vk::raii::Semaphore m_RenderFinishedSemaphore = nullptr;
    vk::raii::Fence     m_InFlightFence = nullptr;
    vk::raii::CommandBuffer m_CommandBuffer = nullptr;
    
    // Friend
    friend class VulkanSwapchain;

    // Private member functions
    void CreateCommandBuffer(VulkanDevice& device);
    void CreateSyncObjects(VulkanDevice& device);
};

#endif // RENDERER_VULKAN_VULKANFRAME
