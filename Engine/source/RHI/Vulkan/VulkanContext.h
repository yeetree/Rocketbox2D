#ifndef RHI_VULKAN_VULKANCONTEXT
#define RHI_VULKAN_VULKANCONTEXT

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Platform/IWindow.h"

#include "RHI/Vulkan/IVulkanGraphicsBridge.h"
#include "RHI/Vulkan/VulkanQueue.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    class ENGINE_EXPORT VulkanContext
    {
    private:
        Ref<IVulkanGraphicsBridge> m_Bridge;
        vk::raii::Context m_Context;
        vk::raii::Instance m_Instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
        vk::raii::SurfaceKHR m_Surface = nullptr;
        vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
        vk::raii::Device m_Device = nullptr;
        VulkanQueue m_GraphicsQueue;

        // Debug callback
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);

        void CreateInstance();
        void SetupDebugMessanger();
        void CreateSurface(Ref<IWindow> window);
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        
    public:
        VulkanContext(Ref<IVulkanGraphicsBridge> bridge, Ref<IWindow> window);
        ~VulkanContext();

        // No copying
        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;

        vk::raii::Instance& GetInstance() { return m_Instance; }
        vk::raii::SurfaceKHR& GetSurface() { return m_Surface; }
        vk::raii::PhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }
        vk::raii::Device& GetDevice() { return m_Device; }
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANCONTEXT
