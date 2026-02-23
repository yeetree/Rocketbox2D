#ifndef RENDERER_VULKAN_VULKANDEVICE
#define RENDERER_VULKAN_VULKANDEVICE

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

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
    VmaAllocator& GetAllocator();

private:
    // Vulkan members
    vk::raii::Device m_Device = nullptr;
    uint32_t m_QueueIndex = ~0;
    vk::raii::Queue m_Queue = nullptr;
    vk::raii::CommandPool m_CommandPool = nullptr;
    VmaAllocator m_Allocator;;

    // Private helper functions
    void CreateLogicalDevice(VulkanContext& context);
    void CreateCommandPool();
    void CreateAllocator(VulkanContext& context);
};

#endif // RENDERER_VULKAN_VULKANDEVICE
