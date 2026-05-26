#include "Platform/SDL3/Vulkan/SDL3VulkanGraphicsBridge.h"

#include "Engine/Core/Assert.h"

#include "Platform/SDL3/SDL3Window.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace Engine
{

    VkSurfaceKHR SDL3VulkanGraphicsBridge::CreateSurface(VkInstance instance, VkPhysicalDevice pd, uint32_t presentQueueIndex, IWindow* window)
    {
        ENGINE_CORE_ASSERT(window != nullptr, "SDL3 + Vulkan: CreateSurface(): window is nullptr!");
        ENGINE_CORE_ASSERT(window->GetPlatform() == Platform::SDL, "SDL3 + Vulkan: CreateSurface(): window is not for SDL3!");
        ENGINE_CORE_ASSERT(window->GetAPI() == GraphicsAPI::Vulkan, "SDL3 + Vulkan: CreateSurface(): window is not for Vulkan!");

        SDL3Window* sdlWindow = static_cast<SDL3Window*>(window);

        VkSurfaceKHR surface;

        if (!SDL_Vulkan_CreateSurface(sdlWindow->GetSDLWindow(), instance, NULL, &surface)) {
            throw std::runtime_error(std::format("SDL3 + Vulkan: CreateSurface(): Could not create surface: Error: {0}", SDL_GetError()));
        }
        

        VkBool32 presentSupport = VK_FALSE;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
            pd, 
            presentQueueIndex, 
            surface, 
            &presentSupport
        );

        if(result != VK_SUCCESS || !presentSupport)
        {
            throw std::runtime_error("SDL3 + Vulkan: CreateSurface(): Could not create surface: Presentation queue family does not support presentation onto new surface!");
        }

        return surface;
    }

    // please kindy see source/RHI/Vulkan/IVulkanGraphicsBridge.h
    /*
    void SDL3VulkanGraphicsBridge::DestroySurface(VkInstance instance, VkSurfaceKHR surface)
    {
        SDL_Vulkan_DestroySurface(instance, surface, NULL);
    }
    */
    
    std::vector<const char*> SDL3VulkanGraphicsBridge::GetInstanceExtensions()
    {
        // Get required extensions for SDL
        uint32_t SDLExtensionCount = 0;
        char const * const * SDLExtensions = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount);
        if (!SDLExtensions) {
            throw std::runtime_error(std::format("SDL3 + Vulkan: GetInstanceExtensions(): Could not get instance extensions: Error: {0}", SDL_GetError()));
        }
        return std::vector<const char*>(SDLExtensions, SDLExtensions + SDLExtensionCount);
    }

    VkSurfaceKHR* SDL3VulkanGraphicsBridge::CreateDummySurface(VkInstance instance)
    {
        // Create dummy window
        m_DummyWindow = SDL_CreateWindow(
            "EngineSDL3VulkanDummyWindow",
            1, 1,
            SDL_WINDOW_HIDDEN | SDL_WINDOW_VULKAN
        );

        if(!m_DummyWindow)
        {
            throw std::runtime_error(std::format("SDL3 + Vulkan: CreateDummySurface(): Could not create a dummy window: Error: {0}", SDL_GetError()));
        }

        // Create dummy surface
        if (!SDL_Vulkan_CreateSurface(m_DummyWindow, instance, NULL, &m_DummySurface)) {
            throw std::runtime_error(std::format("SDL3 + Vulkan: CreateDummySurface(): Could not create a dummy surface: Error: {0}", SDL_GetError()));
        }

        return &m_DummySurface;
    }

    void SDL3VulkanGraphicsBridge::DestroyDummySurface(VkInstance instance)
    {
        // Destroy
        if(m_DummySurface)
        {
            SDL_Vulkan_DestroySurface(instance, m_DummySurface, NULL);
        }
        if(m_DummyWindow)
        {
            SDL_DestroyWindow(m_DummyWindow);
        }
        m_DummySurface = nullptr;
        m_DummyWindow = nullptr;
    }
} // namespace Engine
