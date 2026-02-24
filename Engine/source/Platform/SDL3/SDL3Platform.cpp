#include "Platform/SDL3/SDL3Platform.h"
#include "Platform/SDL3/SDL3Window.h"

#include "Engine/Core/Log.h"

#include <SDL3/SDL.h>
#include <format>

namespace Engine
{
    SDL3Platform::SDL3Platform(Scope<IGraphicsBridge> graphicsBridge)
        : m_GraphicsBridge(std::move(graphicsBridge)) 
    {
        // Initialize SDL
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
            throw std::runtime_error(std::format("SDL3: SDL could not initialize! Error: {0}", SDL_GetError()));
            return; // Failure
        }
    };

    SDL3Platform::~SDL3Platform() {
        SDL_Quit();
    }

    Scope<IWindow> SDL3Platform::CreateWindow(const WindowProperties& properties) {
        return CreateScope<SDL3Window>(properties);
    }

    IGraphicsBridge& SDL3Platform::GetGraphicsBridge() {
        return *m_GraphicsBridge;
    }
} // namespace Engine
