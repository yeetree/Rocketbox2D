#include "Platform/SDL3/Vulkan/SDL3VulkanGraphicsBridge.h"
#include "Platform/SDL3/SDL3Window.h"
#include "Engine/Core/Assert.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace Engine {
    VkSurfaceKHR SDL3VulkanGraphicsBridge::CreateSurface(VkInstance instance, IWindow* window) {
        ENGINE_CORE_ASSERT(window != nullptr, "SDL3 + Vulkan: CreateSurface(): window is nullptr!");
        
        // Note: This is completely safe*
        //       IPlatform::Create() automatically selects platform
        //       and these functions can only be called by Vulkan RHI classes
        //       and this class can only be created by IPlatform AUTOMATICALLY
        //
        //       *Unless somehow you manually create an incorrect backend
        //        As long as IPlatform and IGraphicsDevice are created with the same
        //        GraphicsAPI, this should be fine.

        SDL3Window* sdlWindow = static_cast<SDL3Window*>(window);

        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(sdlWindow->GetSDLWindow(), instance, NULL, &surface)) {
            throw std::runtime_error(std::format("SDL3 + Vulkan: Could not create surface: Error: {0}", SDL_GetError()));
        }
        return surface;
    };

    std::vector<const char*> SDL3VulkanGraphicsBridge::GetRequiredExtensions() {
        // Get required extensions for SDL
        uint32_t SDLExtensionCount = 0;
        char const * const * SDLExtensions = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount);
        if (!SDLExtensions) {
            throw std::runtime_error(std::format("SDL3 + Vulkan: Could not get required extensions: Error: {0}", SDL_GetError()));
            return {}; // Failure
        }
        return std::vector<const char*>(SDLExtensions, SDLExtensions + SDLExtensionCount);
    };
}