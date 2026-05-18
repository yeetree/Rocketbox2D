#include "Platform/SDL3/SDL3Platform.h"
#include "Platform/SDL3/SDL3Window.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    SDL3Platform::SDL3Platform()
    {
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
            throw std::runtime_error(std::format("SDL3: SDL could not initialize! Error: {0}", SDL_GetError()));
            return; // Failure
        }

        // Get base path
        const char * basePath = SDL_GetBasePath();

        if(basePath == NULL)
        {
            throw std::runtime_error(std::format("SDL3: Could not get base path! Error: {0}", SDL_GetError()));
            return; // Failure
        }

        m_BasePath = basePath;
    };

    SDL3Platform::~SDL3Platform()
    {
        SDL_Quit();
    }
    
    Scope<IWindow> SDL3Platform::CreateWindow(const WindowProperties& properties)
    {
        Scope<SDL3Window> window = CreateScope<SDL3Window>(properties);
        return window;
    }

} // namespace Engine
