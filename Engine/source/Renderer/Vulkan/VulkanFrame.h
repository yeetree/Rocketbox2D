#ifndef RENDERER_VULKAN_VULKANFRAME
#define RENDERER_VULKAN_VULKANFRAME

#include <vulkan/vulkan_raii.hpp>
#include "Renderer/Vulkan/VulkanDescriptorManager.h"

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
    vk::raii::Fence& GetInFlightFence();
    vk::raii::CommandBuffer& GetCommandBuffer();
    VulkanDescriptorManager& GetDescriptorManager();

    void Reset();

private:
    // Vulkan members
    vk::raii::Fence     m_InFlightFence = nullptr;
    vk::raii::CommandBuffer m_CommandBuffer = nullptr;

    VulkanDescriptorManager m_DescriptorManager;
    
    // Friend
    friend class VulkanSwapchain;

    // Private member functions
    void CreateCommandBuffer(VulkanDevice& device);
    void CreateSyncObjects(VulkanDevice& device);
};

#endif // RENDERER_VULKAN_VULKANFRAME
