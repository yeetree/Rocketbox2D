#ifndef RENDERER_VULKAN_VULKANDEVICE
#define RENDERER_VULKAN_VULKANDEVICE

#include <vulkan/vulkan_raii.hpp>

// fwd
class VulkanContext;

class VulkanDevice {
public:
    // Constructor: Creates an VulkanDevice
    VulkanDevice(VulkanContext& context);
    ~VulkanDevice();

    // Prevent copying
    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    // Getters
    vk::raii::Device& GetDevice();
    vk::raii::Queue& GetQueue();
    vk::raii::CommandPool& GetCommandPool();

private:
    // Vulkan members
    vk::raii::Device m_Device = nullptr;
    uint32_t m_QueueIndex = ~0;
    vk::raii::Queue m_Queue = nullptr;
    vk::raii::CommandPool m_CommandPool = nullptr;

    // Private helper functions
    void CreateLogicalDevice(VulkanContext& context);
    void CreateCommandPool();
};

#endif // RENDERER_VULKAN_VULKANDEVICE
