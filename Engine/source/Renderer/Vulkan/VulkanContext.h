#ifndef RENDERER_VULKAN_VULKANCONTEXT
#define RENDERER_VULKAN_VULKANCONTEXT

#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>

// Manages Vulkan Context
class VulkanContext {
public:
    // Constructor: Creates an VulkanContext
    VulkanContext(SDL_Window* window);
    ~VulkanContext();

    // Prevent copying
    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    // Getters
    vk::raii::PhysicalDevice& GetPhysicalDevice();
    vk::raii::SurfaceKHR& GetSurface();
    SDL_Window* GetWindow();

private:
    // Debug Callback
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);

    // Vulkan members
    vk::raii::Context m_Context;
    vk::raii::Instance m_Instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    vk::raii::SurfaceKHR m_Surface = nullptr;
    vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;

    // SDL members
    SDL_Window* m_Window = nullptr;

    // Private helper functions
    void CreateInstance();
    void SetupDebugMessanger();
    void CreateSurface();
    void PickPhysicalDevice();
};

#endif // RENDERER_VULKAN_VULKANCONTEXT
