#include "Platform/SDL3/SDL3Window.h"

#include <format>

namespace Engine
{
    SDL3Window::SDL3Window(const WindowProperties& properties)
        : m_Width(properties.width), m_Height(properties.height), m_VSync(properties.vsync)
    {

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
    };

    

    void SDL3Window::OnUpdate() {

    }

    unsigned int SDL3Window::GetWidth() { return m_Width; };
    unsigned int SDL3Window::GetHeight() { return m_Height; };

    void SDL3Window::SetVSync(bool vsync) { m_VSync = vsync; };
    bool SDL3Window::IsVSync() { return m_VSync; };

    SDL_Window* SDL3Window::GetSDLWindow() { return m_Window; }

    SDL3Window::~SDL3Window() {
        if(m_Window)
            SDL_DestroyWindow(m_Window);
    };
} // namespace Engine
