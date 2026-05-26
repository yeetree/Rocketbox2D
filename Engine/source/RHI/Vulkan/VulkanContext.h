#ifndef RHI_VULKAN_VULKANCONTEXT
#define RHI_VULKAN_VULKANCONTEXT

#include "engine_export.h"

#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "RHI/Vulkan/VulkanQueue.h"

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

namespace Engine
{
    class ENGINE_EXPORT VulkanContext
    {
    private:
        vk::raii::Context m_Context;
        vk::raii::Instance m_Instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
        vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
        vk::raii::Device m_Device = nullptr;
        // TODO: Vulkan: Put command pool somewhere better
        vk::raii::CommandPool m_CommandPool = nullptr;
        VmaAllocator m_Allocator;
        VulkanQueue m_GraphicsQueue;

        void CreateInstance(IVulkanGraphicsBridge* bridge);
        void SetupDebugMessenger();
        void PickPhysicalDevice();
        void CreateLogicalDevice(IVulkanGraphicsBridge* bridge);
        void CreateAllocator();

    public:
        VulkanContext(IVulkanGraphicsBridge* bridge);
        ~VulkanContext();

        vk::raii::Instance& GetInstance() { return m_Instance; }
        vk::raii::PhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }
        vk::raii::Device& GetDevice() { return m_Device; }
        VmaAllocator& GetAllocator() { return m_Allocator; }
        VulkanQueue& GetGraphicsQueue() { return m_GraphicsQueue; }
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANCONTEXT
