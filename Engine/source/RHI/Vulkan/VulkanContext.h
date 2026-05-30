#ifndef RHI_VULKAN_VULKANCONTEXT
#define RHI_VULKAN_VULKANCONTEXT

#include "engine_export.h"

#include "RHI/Vulkan/VulkanQueue.h"

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

namespace Engine::RHI::Vulkan
{
    // Forward
    class IVulkanGraphicsBridge;

    class ENGINE_EXPORT VulkanContext
    {
    private:
        vk::raii::Context m_Context;
        vk::raii::Instance m_Instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
        vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
        vk::PhysicalDeviceProperties m_PhysicalDeviceProperties;
        vk::raii::Device m_Device = nullptr;
        VmaAllocator m_Allocator;
        // TODO: Vulkan: Separate graphics and presentation queues
        VulkanQueue m_GraphicsQueue;

        void CreateInstance(IVulkanGraphicsBridge* bridge);
        void SetupDebugMessenger();
        void PickPhysicalDevice();
        void CreateLogicalDevice(IVulkanGraphicsBridge* bridge);
        void CreateAllocator();

    public:
        VulkanContext(IVulkanGraphicsBridge* bridge);
        ~VulkanContext();

        vk::raii::Instance&           GetInstance() { return m_Instance; }
        vk::raii::PhysicalDevice&     GetPhysicalDevice() { return m_PhysicalDevice; }
        vk::PhysicalDeviceProperties& GetPhysicalDeviceProperties() { return m_PhysicalDeviceProperties; }
        vk::raii::Device&             GetDevice() { return m_Device; }
        VmaAllocator&                 GetAllocator() { return m_Allocator; }
        VulkanQueue&                  GetGraphicsQueue() { return m_GraphicsQueue; }
    };
} // namespace Engine::RHI::Vulkan


#endif // RHI_VULKAN_VULKANCONTEXT
