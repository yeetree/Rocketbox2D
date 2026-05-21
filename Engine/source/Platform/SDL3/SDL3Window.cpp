#include "Platform/SDL3/SDL3Window.h"

#include <format>

namespace Engine
{
    SDL3Window::SDL3Window(const WindowProperties& props) : IWindow(Platform::SDL, props.api), m_Width(props.width), m_Height(props.height)
    {
        SDL_WindowFlags flags = 0;

        if(props.resizable)
            flags |= SDL_WINDOW_RESIZABLE;
        
        switch(props.api) {
            case GraphicsAPI::Vulkan: flags |= SDL_WINDOW_VULKAN; break;
        }

        m_Window = SDL_CreateWindow(props.title.c_str(), m_Width, m_Height, flags);
        if(m_Window == nullptr)
        {
            throw std::runtime_error(std::format("SDL3: Window could not be created! Error: {0}", SDL_GetError()));
            return; // Failure
        }
    
        // TEMP:
        // Draw one frame to show window
        SDL_Renderer* renderer = SDL_CreateRenderer(m_Window, NULL);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_DestroyRenderer(renderer);
    };

    SDL3Window::~SDL3Window()
    {
        if(m_Window)
            SDL_DestroyWindow(m_Window);
    };

    unsigned int SDL3Window::GetWidth() 
    {
        return m_Width;
    };

    unsigned int SDL3Window::GetHeight()
    {
        return m_Height;
    };

    // SDL3Window specific -- not declared in IWindow
    SDL_Window* SDL3Window::GetSDLWindow() const
    {
        return m_Window;
    };
} // namespace Engine
