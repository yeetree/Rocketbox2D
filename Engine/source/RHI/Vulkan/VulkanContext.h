#ifndef RHI_VULKAN_VULKANCONTEXT
#define RHI_VULKAN_VULKANCONTEXT

#include "engine_export.h"

#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "RHI/Vulkan/VulkanQueue.h"

#include <vulkan/vulkan_raii.hpp>

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

        VulkanQueue m_GraphicsQueue;

        void CreateInstance(IVulkanGraphicsBridge* bridge);
        void SetupDebugMessenger();
        void PickPhysicalDevice();
        void CreateLogicalDevice(IVulkanGraphicsBridge* bridge);

    public:
        VulkanContext(IVulkanGraphicsBridge* bridge);
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANCONTEXT
