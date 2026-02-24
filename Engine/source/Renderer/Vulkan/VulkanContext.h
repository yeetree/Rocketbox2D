#ifndef RENDERER_VULKAN_VULKANCONTEXT
#define RENDERER_VULKAN_VULKANCONTEXT

#include <vulkan/vulkan_raii.hpp>

// fwd
namespace Engine {
    class IVulkanGraphicsBridge;
    class IWindow;
}

// Manages Vulkan Context
class VulkanContext {
public:
    // Constructor: Creates an VulkanContext
    VulkanContext(Engine::IVulkanGraphicsBridge* graphicsBridge, Engine::IWindow* window);
    ~VulkanContext();

    // Prevent copying
    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    // Getters
    vk::raii::Instance& GetInstance();
    vk::raii::PhysicalDevice& GetPhysicalDevice();
    vk::raii::SurfaceKHR& GetSurface();

private:
    // Debug Callback
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);

    // Vulkan members
    vk::raii::Context m_Context;
    vk::raii::Instance m_Instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    vk::raii::SurfaceKHR m_Surface = nullptr;
    vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;

    // Private helper functions
    void CreateInstance(Engine::IVulkanGraphicsBridge* graphicsBridge);
    void SetupDebugMessanger();
    void CreateSurface(Engine::IVulkanGraphicsBridge* graphicsBridge, Engine::IWindow* window);
    void PickPhysicalDevice();
};

#endif // RENDERER_VULKAN_VULKANCONTEXT
