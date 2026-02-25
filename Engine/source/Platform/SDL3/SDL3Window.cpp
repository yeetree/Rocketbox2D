#include "Platform/SDL3/SDL3Window.h"
#include "Platform/SDL3/SDL3Platform.h"

#include "Engine/Core/Assert.h"

#include <format>

namespace Engine
{
    SDL3Window::SDL3Window(SDL3Platform* platform, const WindowProperties& properties)
        : m_Platform(platform), m_Width(properties.width), m_Height(properties.height)
    {
        ENGINE_CORE_ASSERT(m_Platform != nullptr, "SDL3: SDL3Window(): platform is nullptr!");

        // Get flags
        SDL_WindowFlags flags = 0;
        if(properties.resizeable)
            flags |= SDL_WINDOW_RESIZABLE;

        switch(properties.api) {
            case GraphicsAPI::Vulkan: flags |= SDL_WINDOW_VULKAN; break;
        }

        m_Window = SDL_CreateWindow(properties.title.c_str(), m_Width, m_Height, flags);
        if(m_Window == nullptr)
        {
            throw std::runtime_error(std::format("SDL3: Window could not be created! Error: {0}", SDL_GetError()));
            return; // Failure
        }

        // Map SDL WindowID to SDL3Window*
        m_SDLID = SDL_GetWindowID(m_Window);
        m_Platform->m_WindowMap[m_SDLID] = this;
    };

    unsigned int SDL3Window::GetWidth() { return m_Width; };
    unsigned int SDL3Window::GetHeight() { return m_Height; };

    SDL_Window* SDL3Window::GetSDLWindow() { return m_Window; }

    SDL3Window::~SDL3Window() {
        // Remove from map
        m_Platform->m_WindowMap.erase(m_SDLID);
        if(m_Window)
            SDL_DestroyWindow(m_Window);
    };
} // namespace Engine
